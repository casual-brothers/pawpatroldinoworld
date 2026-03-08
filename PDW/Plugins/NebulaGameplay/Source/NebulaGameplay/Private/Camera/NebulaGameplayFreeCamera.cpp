#include "Camera/NebulaGameplayFreeCamera.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"


#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

const FName ANebulaGameplayFreeCamera::FREECAMERA_TAG = FName("FreeCamera");




ANebulaGameplayFreeCamera::ANebulaGameplayFreeCamera(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostPhysics;

	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this,TEXT("SphereComponent"));
	if (SphereComponent)
	{
		RootComponent = SphereComponent;
		SphereComponent->bDynamicObstacle = false;
		SphereComponent->SetCollisionProfileName(FName("BlockAll"));
		SphereComponent->SetTickGroup(ETickingGroup::TG_PostPhysics);
	}
	CameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this,TEXT("FreeCameraComponent"));
	if (CameraComponent)
	{
		CameraComponent->SetupAttachment(SphereComponent);
		CameraComponent->PostProcessSettings.bOverride_MotionBlurAmount = true;
		CameraComponent->PostProcessSettings.MotionBlurAmount = 0.f;
		CameraComponent->FieldOfView = 70.f;
		CameraComponent->bConstrainAspectRatio = false;
		CameraComponent->SetTickGroup(ETickingGroup::TG_PostPhysics);
	}
}

void ANebulaGameplayFreeCamera::BeginPlay()
{
	Super::BeginPlay();
	InitialFov = CameraComponent->FieldOfView;
	this->Tags.Add(FREECAMERA_TAG);
}

void ANebulaGameplayFreeCamera::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime,TickType,ThisTickFunction);

	if (bCameraInputEnabled && CameraTarget)
	{
		float RealDeltaTime = GetWorld()->GetTime().GetDeltaRealTimeSeconds();
		HandleMovement(RealDeltaTime);
		HandleRotation(RealDeltaTime);
		HandleZoom(RealDeltaTime);
		HandleTime(RealDeltaTime);
	}
}

void ANebulaGameplayFreeCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	Super::EndPlay(EndPlayReason);
}

void ANebulaGameplayFreeCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (PlayerInputComponent)
	{
		InputComponentRef = PlayerInputComponent;
		PlayerInputComponent->BindAxis(LateralMovementInputName).bExecuteWhenPaused=true;
		PlayerInputComponent->BindAxis(LongitudinalMovementInputName).bExecuteWhenPaused=true;
		PlayerInputComponent->BindAxis(UpMovementInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(DownMovementInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(PitchRotationInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(YawRotationInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(RollClockWiseRotationInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(RollAntiClockWiseRotationInputName).bExecuteWhenPaused = true;

		PlayerInputComponent->BindAxis(FreeCameraUpDpadInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(FreeCameraDownDpadInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(FreeCameraRightDpadInputName).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAxis(FreeCameraLeftDpadInputName).bExecuteWhenPaused = true;

		PlayerInputComponent->BindAction(FreeCameraFreezeTimeInputName, EInputEvent::IE_Pressed, this, &ANebulaGameplayFreeCamera::SwitchFreezeTime).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAction(FreeCameraFollowTargetInputName, EInputEvent::IE_Pressed, this, &ANebulaGameplayFreeCamera::SwitchFollowActor).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAction(FreeCameraUfoModeInputName, EInputEvent::IE_Pressed, this, &ANebulaGameplayFreeCamera::SwitchUfoMode).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAction(FreeCameraResetRollInputName, EInputEvent::IE_Pressed, this, &ANebulaGameplayFreeCamera::ResetRoll).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAction(FreeCameraSwitchCameraInputName, EInputEvent::IE_Pressed, this, &ANebulaGameplayFreeCamera::SwitchCamera).bExecuteWhenPaused = true;
		PlayerInputComponent->BindAction(FreeCameraSwitchDpadInputName, EInputEvent::IE_Pressed, this, &ANebulaGameplayFreeCamera::SwitchDpadUsage).bExecuteWhenPaused = true;
	}
}

void ANebulaGameplayFreeCamera::InitializeFreeCamera(AActor* ActorTarget, FTransform InitialTransform)
{
	CameraTarget = ActorTarget;
	SetActorTransform(InitialTransform);
	CurrentRelativeLocation = InitialTransform.GetLocation();

	// Trick to activate follow actor correctly
	bFollowTarget = false;
	SwitchFollowActor();
	bCollisionInitialized = false;
	ResolveCollisions(FVector::ZeroVector);
	CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance = InitialDof;
	CameraComponent->PostProcessSettings.DepthOfFieldSensorWidth = InitialSensorWidth;

	RemoveAllIMCFromController();	
}


void ANebulaGameplayFreeCamera::ToggleUI(bool bEnable)
{
	TArray<UUserWidget*> Widgets{};
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Widgets , UUserWidget::StaticClass(), true);
	for (auto Widget : Widgets)
	{
		Widget->SetVisibility(bEnable ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void ANebulaGameplayFreeCamera::UpdateFollowActor(AActor* ActorTarget)
{
	CameraTarget = ActorTarget;
	if (!bSwitchCamera)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(this);
	}
}

void ANebulaGameplayFreeCamera::SwitchFollowActor()
{
	bFollowTarget = !bFollowTarget;
	if (bFollowTarget)
	{
		CurrentRelativeLocation -= CameraTarget->GetActorLocation();
	}
	else
	{
		CurrentRelativeLocation += CameraTarget->GetActorLocation();
	}
}

void ANebulaGameplayFreeCamera::SwitchCamera()
{
	bSwitchCamera = !bSwitchCamera;
	if (bSwitchCamera)
	{	
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(CameraTarget);		
		UGameplayStatics::SetForceDisableSplitscreen(this, false);
		if (!UGameplayStatics::GetPlayerController(GetWorld(), 2))
		{
			static const auto cvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.HZBOcclusion"));
			cvar->Set(0);
		}
		ToggleUI(true);
	}
	else
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(this);	
		UGameplayStatics::SetForceDisableSplitscreen(this, true);
		if (!UGameplayStatics::GetPlayerController(GetWorld(), 2))
		{
			static const auto cvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.HZBOcclusion"));
			cvar->Set(1);
		}
		ToggleUI(false);
	}
}

void ANebulaGameplayFreeCamera::SwitchDpadUsage()
{
	bSwitchDpadUsage = !bSwitchDpadUsage;
}

void ANebulaGameplayFreeCamera::SwitchUfoMode()
{
	bUfoMode = !bUfoMode;
}

void ANebulaGameplayFreeCamera::ResetRoll()
{
	FRotator NewRotation = FRotator(GetTransform().GetRotation());
	NewRotation.Roll = 0;
	SetActorRotation(NewRotation);
}



void ANebulaGameplayFreeCamera::HandleMovement(float DeltaTime)
{
	if(InputComponentRef)
	{
		if (!bSwitchDpadUsage)
		{
			float Accelerate = InputComponentRef->GetAxisValue(FreeCameraRightDpadInputName);
			float Slow = InputComponentRef->GetAxisValue(FreeCameraLeftDpadInputName);

			if (FMath::Abs(Accelerate) > AxisDeadzone)
			{
				MovementSpeed += Accelerate * DeltaTime * MovementSpeedMultiplier * MovementSpeed;
				MovementSpeed = FMath::Clamp(MovementSpeed, MinMovementSpeed, MaxMovementSpeed);
			}
			//LongitudinaLMovement
			if (FMath::Abs(Slow) > AxisDeadzone)
			{
				MovementSpeed -= Slow * DeltaTime * MovementSpeedMultiplier * MovementSpeed;
				MovementSpeed = FMath::Clamp(MovementSpeed, MinMovementSpeed, MaxMovementSpeed);
			}
		}

		float LateralValue = InputComponentRef->GetAxisValue(LateralMovementInputName);
		float LongitudinalValue = InputComponentRef->GetAxisValue(LongitudinalMovementInputName);
		float UpValue = InputComponentRef->GetAxisValue(UpMovementInputName);
		float DownValue = InputComponentRef->GetAxisValue(DownMovementInputName);
		FVector DesiredOffset = FVector::ZeroVector;
		//LateralMovement
		if(FMath::Abs(LateralValue)>AxisDeadzone)
		{ 
			FVector RightVector = bUfoMode? FPlane::VectorPlaneProject(GetActorRightVector(), FVector::UpVector).GetSafeNormal() : GetActorRightVector();
			DesiredOffset += LateralValue * DeltaTime * MovementSpeed * RightVector;
		}
		//LongitudinaLMovement
		if(FMath::Abs(LongitudinalValue)>AxisDeadzone)
		{ 
			FVector ForwardVector = bUfoMode? FPlane::VectorPlaneProject(GetActorForwardVector(), FVector::UpVector).GetSafeNormal() : GetActorForwardVector();
			DesiredOffset += LongitudinalValue *DeltaTime * MovementSpeed * ForwardVector;
		}
		if (FMath::Abs(UpValue) > AxisDeadzone)
		{
			DesiredOffset += UpValue * DeltaTime * MovementSpeed * (bVerticalMovementRelativeToWorld ? FVector::UpVector : GetActorUpVector());
		}
		else if (FMath::Abs(DownValue) > AxisDeadzone)
		{
			DesiredOffset -= DownValue * DeltaTime * MovementSpeed * (bVerticalMovementRelativeToWorld ? FVector::UpVector : GetActorUpVector());
		}

		if (bDoCollisionCheck)
		{
			ResolveCollisions(DesiredOffset);
		}
		else
		{
			CurrentRelativeLocation += DesiredOffset;
			SetActorLocation((bFollowTarget ? CameraTarget->GetActorLocation() : FVector::ZeroVector) + CurrentRelativeLocation, true);
		}
	}
}

void ANebulaGameplayFreeCamera::HandleRotation(float DeltaTime)
{
	if (InputComponentRef)
	{
		float YawValue = InputComponentRef->GetAxisValue(YawRotationInputName);
		float PitchValue = InputComponentRef->GetAxisValue(PitchRotationInputName);
		float RollClockWiseValue = InputComponentRef->GetAxisValue(RollClockWiseRotationInputName);
		float RollAntiClockWiseValue = InputComponentRef->GetAxisValue(RollAntiClockWiseRotationInputName);

		float YawOffset = 0.f;
		float PitchOffset = 0.f;
		float RollOffset = 0.f;
		if (FMath::Abs(YawValue) > AxisDeadzone)
		{
			YawOffset = YawValue * DeltaTime * RotationSpeed;
		}
		if (FMath::Abs(PitchValue) > AxisDeadzone)
		{
			PitchOffset = PitchValue * DeltaTime * RotationSpeed;
		}
		if(!bSwitchDpadUsage)
		{
			if (FMath::Abs(RollClockWiseValue) > AxisDeadzone)
			{
				RollOffset += RollClockWiseValue * DeltaTime * RotationSpeed;
			}
			if (FMath::Abs(RollAntiClockWiseValue) > AxisDeadzone)
			{
				RollOffset -= RollAntiClockWiseValue * DeltaTime * RotationSpeed;
			}
		}

		FRotator DesiredOffset = FRotator(PitchOffset,YawOffset,RollOffset);

		//FRotator NewRotation = (GetRootComponent()->GetRelativeTransform().GetRotation() * DesiredOffset.Quaternion());

		FRotator NewRotation = FRotator(GetTransform().GetRotation());
		NewRotation.Pitch += PitchOffset;
		NewRotation.Roll += RollOffset;
		NewRotation.Yaw += YawOffset;
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch,-89.f,89.f);
		SetActorRotation(NewRotation);
	}
}

void ANebulaGameplayFreeCamera::HandleZoom(float Deltatime)
{
	if (InputComponentRef && CameraComponent && bSwitchDpadUsage)
	{
		
		float FovInValue = InputComponentRef->GetAxisValue(RollClockWiseRotationInputName);
		float FovOutValue = InputComponentRef->GetAxisValue(RollAntiClockWiseRotationInputName);
		float FovOffset = 0.f;

		if (FMath::Abs(FovInValue) > AxisDeadzone)
		{
			FovOffset-=FovInValue * Deltatime *FovSpeed;
		}
		if (FMath::Abs(FovOutValue) > AxisDeadzone)
		{
			FovOffset+= FovOutValue * Deltatime * FovSpeed;
		}
		float NewFov = CameraComponent->FieldOfView + FovOffset; 
		NewFov = FMath::Clamp(NewFov, MinFov, MaxFov);
		CameraComponent->SetFieldOfView(NewFov);
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, *FString::Printf(TEXT("FOV: %3.2f"), NewFov));

		float DofInValue = InputComponentRef->GetAxisValue(FreeCameraDownDpadInputName);
		float DofOutValue = InputComponentRef->GetAxisValue(FreeCameraUpDpadInputName);
		float DofOffset = 0.f;

		if (FMath::Abs(DofInValue) > AxisDeadzone)
		{
			DofOffset-=DofInValue * Deltatime *DofSpeed;
		}
		if (FMath::Abs(DofOutValue) > AxisDeadzone)
		{
			DofOffset+= DofOutValue * Deltatime * DofSpeed;
		}
		if (DofOffset != 0.f)
		{
			CameraComponent->PostProcessSettings.bOverride_DepthOfFieldFocalDistance =  true; 
		}
		CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance = FMath::Clamp(CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance + DofOffset* CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance, 1.0f, MaxDof);
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, *FString::Printf(TEXT("DOF: %3.2f"), CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance));
		
		float SensorWidthInValue = InputComponentRef->GetAxisValue(FreeCameraLeftDpadInputName);
		float SensorWidthOutValue = InputComponentRef->GetAxisValue(FreeCameraRightDpadInputName);
		float SensorWidthOffset = 0.f;

		if (FMath::Abs(SensorWidthInValue) > AxisDeadzone)
		{
			SensorWidthOffset-=SensorWidthInValue * Deltatime *SensorWidthSpeed;
		}
		if (FMath::Abs(SensorWidthOutValue) > AxisDeadzone)
		{
			SensorWidthOffset+= SensorWidthOutValue * Deltatime * SensorWidthSpeed;
		}
		if (SensorWidthOffset != 0.f)
		{
			CameraComponent->PostProcessSettings.bOverride_DepthOfFieldSensorWidth =  true; 
		}
		CameraComponent->PostProcessSettings.DepthOfFieldSensorWidth = FMath::Clamp(CameraComponent->PostProcessSettings.DepthOfFieldSensorWidth + SensorWidthOffset *CameraComponent->PostProcessSettings.DepthOfFieldSensorWidth , 1.0f, MaxSensorWidth); 
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, *FString::Printf(TEXT("SWH: %3.2f"), CameraComponent->PostProcessSettings.DepthOfFieldSensorWidth));

	}
}

void ANebulaGameplayFreeCamera::HandleTime(float Deltatime)
{
	if (InputComponentRef && CameraComponent && !bSwitchDpadUsage)
	{
		float Accelerate = InputComponentRef->GetAxisValue(FreeCameraDownDpadInputName);
		float Slow = InputComponentRef->GetAxisValue(FreeCameraUpDpadInputName);
		float TimeSpeedOffset = 0.f;

		if (FMath::Abs(Accelerate) > AxisDeadzone)
		{
			TimeSpeedOffset+=Accelerate * Deltatime *TimeSpeedMultiplier;
		}
		else if (FMath::Abs(Slow) > AxisDeadzone)
		{
			TimeSpeedOffset -= Slow * Deltatime * TimeSpeedMultiplier;
		}
		TimeScale /= 1 + TimeSpeedOffset;
		TimeScale = FMath::Clamp(TimeScale, MinTimeSpeed, MaxTimeSpeed);
		if (!bFreezeTime)
		{
			GetWorldSettings()->SetTimeDilation(TimeScale);
		}
	}
}

void ANebulaGameplayFreeCamera::SwitchFreezeTime()
{
	bFreezeTime = !bFreezeTime;	
	GetWorldSettings()->SetTimeDilation(bFreezeTime? 0.f : TimeScale);
}

void ANebulaGameplayFreeCamera::ResolveCollisions(FVector DesiredOffset)
{
	if (SphereComponent)
	{
		FComponentQueryParams Params;
		Params.AddIgnoredActor(this);
		FVector StartPoint = bCollisionInitialized ? LastValidPosition : SphereComponent->GetComponentLocation();
		
		FCollisionResponseParams ResponseParam;
		SphereComponent->InitSweepCollisionParams(Params, ResponseParam);
		for (int32 Index = 0; Index < CollisionCheckMaxIterations; Index++)
		{
			FVector EndPoint = StartPoint + DesiredOffset;
			TArray<FHitResult> HitResults;
			bool bHasCollided = GetWorld()->ComponentSweepMulti(HitResults,SphereComponent,StartPoint,EndPoint,SphereComponent->GetComponentRotation(),Params);
			if (bHasCollided)
			{
				for (FHitResult& currentHit : HitResults)
				{
					if (currentHit.bBlockingHit)
					{
						if (currentHit.bStartPenetrating)
						{
							StartPoint+=currentHit.Normal * (currentHit.PenetrationDepth + CollisionCheckCorrection);
							break;
						}
						else
						{
							DesiredOffset -=currentHit.Normal * (currentHit.Normal | DesiredOffset);
						}
					}
				}
			}
			else
			{
				LastValidPosition = EndPoint;
				if (FVector::Distance(LastValidPosition, CameraTarget->GetActorLocation()) > MaxDistanceFromTarget && bCheckdistanceFromTarget)
				{
					LastValidPosition = GetActorLocation();
				}
				bCollisionInitialized = true;
				SetActorLocation(LastValidPosition);
			}
		}
	}
}



// Function to remove all IMCs from the PlayerController
void ANebulaGameplayFreeCamera::RemoveAllIMCFromController()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        // Get the LocalPlayer associated with the PlayerController
        ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
        if (LocalPlayer)
        {
            // Get the Enhanced Input Subsystem
            UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
            if (InputSubsystem)
            {
				// Clear all Input Mapping Contexts from the subsystem
				InputSubsystem->ClearAllMappings();
			}
		}
	}
}