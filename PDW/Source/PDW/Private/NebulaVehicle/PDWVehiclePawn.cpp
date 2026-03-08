// Fill out your copyright notice in the Description page of Project Settings.



#include "NebulaVehicle/PDWVehiclePawn.h"
#include "AbilitySystemComponent.h"
#include "NebulaVehicle/NebulaSuspensionComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "Data/PDWAttributeSet.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/Components/PDWVehicleInteractionComponent.h"
#include "Data/PDWVehicleData.h"
#include "VisualLogger/VisualLogger.h"
#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"

DEFINE_LOG_CATEGORY(PDWVehicleLog);

APDWVehiclePawn::APDWVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;
	VehicleInteractionComponent = CreateDefaultSubobject<UPDWVehicleInteractionComponent>(TEXT("VehicleInteractionComponent"));
	VehicleInteractionComponent->SetupAttachment(RootComponent);
}

void APDWVehiclePawn::Hide(bool bHide)
{
	SetActorEnableCollision(!bHide);
	SetActorHiddenInGame(bHide);
	BP_Hide(bHide);
}

void APDWVehiclePawn::SpeedBoost(const float SpeedMultiplier, const float BoostTimer, const float AccellerationValue)
{
	if (!bBoosting)
	{
		bBoosting = true;
		UPDWGameplayFunctionLibrary::PlayForceFeedback({PDWPlayerController}, UPDWGameplayTagSettings::GetVehicleBoostVibrationTag());		
		ApplyVehicleDebuff(SpeedMultiplier,false,BoostTimer,AccellerationValue);
	}
}

void APDWVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	PrimitiveComponent->OnComponentHit.AddDynamic(this, &APDWVehiclePawn::OnComponenthit);

	if (bUseCustomGravity)
	{
		PrimitiveComponent->SetEnableGravity(false);
	}
	//InitCharacterAbilities();
	VehicleMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	OnVehicleTakeOff.AddUniqueDynamic(this, &ThisClass::OnTakeOff);
	OnVehicleLanding.AddUniqueDynamic(this, &ThisClass::OnLanding);
	if (GetOwner())
	{
		PDWPlayerController = Cast<APawn>(GetOwner())->GetController<APDWPlayerController>();
		if(!PDWPlayerController) return;
		Controller = PDWPlayerController;

		if (PDWPlayerController->GetPDWPlayerState()->GetMiniGameVehiclePhysicsOff())
		{
			SetPhysActiveCarBP(false);
		}

		CustomizeVehicle(PDWPlayerController->GetCurrentPup());
	}
	OriginalReverseSpeed = MaxSpeedInReverse;
}

void APDWVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APDWVehiclePawn::UpdatePhysicsCore(float DeltaTime)
{
	if (!bPhysicsEnabled) return;
	WasInAir = bOnAir;
	PreviousInAirTime = OnAirTime;

	Super::UpdatePhysicsCore(DeltaTime);

	UpdateAirStabilizer(DeltaTime);
	UpdateGravity(DeltaTime);
	UpdateSteering(DeltaTime);

	if ((TorqueToApply != FVector::ZeroVector) || (ForceToApply != FVector::ZeroVector))
	{
		AddTorque(TorqueToApply, true);
		AddForce(ForceToApply);
		TorqueToApply = FVector::ZeroVector;
		ForceToApply = FVector::ZeroVector;
	}

	if (InitialVelocity != FVector::ZeroVector)
	{
		SetLinearVelocity(InitialVelocity, false);
		UE_VLOG(this, PDWVehicleLog, Verbose, TEXT("VEL INIT"));
		InitialVelocity = FVector::ZeroVector;
	}
}

void APDWVehiclePawn::OnComponenthit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float ForceMultiplier = 0.f;
	float TorqueMultiplier = 0.f;
	if (fabs(Hit.ImpactNormal.Z) < 0.9f)
	{
		if (Hit.PhysMaterial != nullptr)
		{
			if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType1 || Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType_Default)
			{
				ForceMultiplier = WallCollisionForceMultiplier;
				TorqueMultiplier = WallCollisionTorqueMultiplier;
			}
			if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType2)
			{
				ForceMultiplier = VehicleCollisionForceMultiplier;
				TorqueMultiplier = VehicleCollisionTorqueMultiplier;
			}
		}

	}
	const FVector LoR = (BodyLocation - Hit.ImpactPoint).GetSafeNormal();
	const float TorqueDir = FMath::Sign(BodyRight | LoR);
	const FVector ForceVector = FVector::VectorPlaneProject(Hit.ImpactNormal, FPlane(BodyUp, 1));
	const float CollisionStr = FMath::Pow(NormalImpulse.Size(), 0.5f) + 20.0f;
	TorqueToApply = CollisionStr * TorqueMultiplier * BodyUp * TorqueDir * 0.1f;
	ForceToApply = CollisionStr * ForceMultiplier * ForceVector * 10000;
	if (Hit.PhysMaterial != nullptr)
		OnCollision.Broadcast(GetSpeedKPH(), Hit.PhysMaterial->SurfaceType, Hit.Location);
	else
		OnCollision.Broadcast(GetSpeedKPH(), EPhysicalSurface::SurfaceType_Default, Hit.Location);
}

void APDWVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//InitCharacterAbilities();
	OnPlayerPossesed.Broadcast();
}

void APDWVehiclePawn::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	if (!bPhysicsEnabled) return;

	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
}

void APDWVehiclePawn::AnalyzeInput(const FVector2D& MovementVector)
{	
	if (bInMiniGame)
	{
		bInMiniGame = false;
		SetAccelerationMultiplier(1.f);
	}
	float MoveSpeed = 0.f;
	if (MovementVector.Length() > 0)
	{
		if (BodyLinearSpeed.X < InPlaceSpeed)
		{
			MoveSpeed = FMath::Clamp(abs(MovementVector.Y), MinSteeringSpeed, 1.f);
		}
		else
		{
			float BaseSpeed = abs(MovementVector.Y) > abs(MovementVector.X) ? abs(MovementVector.Y) : abs(MovementVector.X);
			float MinorSpeed = abs(MovementVector.Y) < abs(MovementVector.X) ? abs(MovementVector.Y) : abs(MovementVector.X);
			MoveSpeed = FMath::Clamp(BaseSpeed + MinorSpeed * 0.5f, 0.f, 1.f);
		}
	}
	
	float MovementSign = FMath::Sign(MovementVector.Y) == 0 ? 1 : FMath::Sign(MovementVector.Y + AdditionalForwardInput);
	float FinalMovement = MoveSpeed * MovementSign;

	//GEngine->AddOnScreenDebugMessage(1000, -1, FColor::Red, *FString::Printf(TEXT("Raw MovementVector: %s"), *MovementVector.ToString()));
	//GEngine->AddOnScreenDebugMessage(1001, -1, FColor::Red, *FString::Printf(TEXT("BodyLinearSpeed.X: %f"), BodyLinearSpeed.X));
	//GEngine->AddOnScreenDebugMessage(1002, -1, FColor::Red, *FString::Printf(TEXT("MoveSpeed: %f"), MoveSpeed));
	//GEngine->AddOnScreenDebugMessage(1003, -1, FColor::Red, *FString::Printf(TEXT("FinalMovement: %f"), FinalMovement));
	SetSteeringInput(MovementVector.X);
	SetThrottleInput(FinalMovement);
}

void APDWVehiclePawn::MiniGameMove(const FVector2D& MovementVector, const FVector& inForwardVector, const FVector& inRightVector)
{
	if (!bInMiniGame)
	{
		bInMiniGame = true;
		SetAccelerationMultiplier(MinigameThrottleMult);
	}

    const FVector VehicleForward = GetActorForwardVector();
    const FVector VehicleRight = GetActorRightVector();
    const FVector VehicleUp = GetActorUpVector();
    FVector NewMovementeVector = FVector(MovementVector.X, -MovementVector.Y, 0.f);

    FVector ProjectedCameraForward = FVector::VectorPlaneProject(inForwardVector, VehicleUp).GetSafeNormal();
    FVector ProjectedCameraRight = FVector::VectorPlaneProject(inRightVector, VehicleUp).GetSafeNormal();

	float Rot = FMath::RadiansToDegrees(FMath::Acos(ProjectedCameraForward.CosineAngle2D(FVector::ForwardVector)));
	NewMovementeVector = NewMovementeVector.RotateAngleAxis(90.f + Rot, FVector::UpVector);
    float ForwardDot = FVector::DotProduct(VehicleForward, NewMovementeVector);
    float RightDot = FVector::DotProduct(VehicleRight, NewMovementeVector);

	float MyThrottle = MovementVector.Length();
	if (ForwardDot < 0)
	{
		float sign  =	FMath::Sign(RightDot);
		RightDot = FMath::Abs(RightDot) + FMath::Abs(ForwardDot);// * 0.5f;
		RightDot *= sign;

		MyThrottle = 0.f;
	}

    //FVector Start = GetActorLocation();
    //DrawDebugDirectionalArrow(GetWorld(), Start, Start + VehicleForward * 1000.f, 100.f, FColor::Blue, false, 0.f, SDPG_Foreground, 100.f);
    //DrawDebugDirectionalArrow(GetWorld(), Start, Start + FVector(NewMovementeVector.X, NewMovementeVector.Y, 0.f) * 1000.f, 100.f, FColor::Red, false, 0.f, SDPG_Foreground, 100.f);
    //DrawDebugDirectionalArrow(GetWorld(), Start, Start + ProjectedCameraForward * 1000.f, 100.f, FColor::Green, false, 0.f, SDPG_Foreground, 100.f);
    //DrawDebugDirectionalArrow(GetWorld(), Start, Start + ProjectedCameraRight * 1000.f, 100.f, FColor::Yellow, false, 0.f, SDPG_Foreground, 100.f);
    //GEngine->AddOnScreenDebugMessage(1000, -1, FColor::Red, *FString::Printf(TEXT("MovementVector: %s"), *NewMovementeVector.ToString()));
    //GEngine->AddOnScreenDebugMessage(1001, -1, FColor::Red, *FString::Printf(TEXT("ForwardDot: %f"), ForwardDot));
    //GEngine->AddOnScreenDebugMessage(10003, -1, FColor::Red, *FString::Printf(TEXT("Rot: %f"), Rot));
    //GEngine->AddOnScreenDebugMessage(1002, -1, FColor::Red, *FString::Printf(TEXT("RightDot: %f"), RightDot));
	
	//SetSteeringInput(RightDot);
	SteeringInput = RightDot;
	SetThrottleInput(MyThrottle);
}

UPDWInteractionComponent* APDWVehiclePawn::GetInteractionComponent()
{
	return VehicleInteractionComponent;
}

APDWPlayerState* APDWVehiclePawn::GetPDWPlayerState()
{
	if (PDWPlayerController)
	{
		return PDWPlayerController->GetPDWPlayerState();
	}
	return nullptr;
}

void APDWVehiclePawn::SetPhysActiveCarBP(bool inActive)
{
	bPhysicsEnabled = inActive;
	PrimitiveComponent->SetCollisionEnabled(inActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	//PrimitiveComponent->SetEnableGravity(inActive);

	for (UNebulaSuspensionComponent* SuspensionComp : SuspensionComponents)
	{
		SuspensionComp->SetComponentTickEnabled(inActive);
	}
	if (!inActive)
	{
		StopCarBP();
	}
}

void APDWVehiclePawn::InitCharacterAbilities()
{
	GASComponent = PDWPlayerController->GetGASComponent();
	if (!IsValid(GASComponent)) return;
	BasicAttributeSet = GASComponent->GetSet<UPDWAttributeSet>();
	GASComponent->GetGameplayAttributeValueChangeDelegate(BasicAttributeSet->GetVehicleAccelerationAttribute()).AddUObject(this, &ThisClass::OnAccelerationChanged);
	GASComponent->GetGameplayAttributeValueChangeDelegate(BasicAttributeSet->GetVehicleSpeedAttribute()).AddUObject(this, &ThisClass::OnTopSpeedChanged);
	GASComponent->GetGameplayAttributeValueChangeDelegate(BasicAttributeSet->GetVehicleTurningAttribute()).AddUObject(this, &ThisClass::OnSteeringChanged);
	FOnAttributeChangeData Placeholder;
	Placeholder.NewValue = BasicAttributeSet->GetVehicleTurning();
	OnSteeringChanged(Placeholder);
	Placeholder.NewValue = BasicAttributeSet->GetVehicleAcceleration();
	OnAccelerationChanged(Placeholder);
	Placeholder.NewValue = BasicAttributeSet->GetVehicleSpeed();
	OnTopSpeedChanged(Placeholder);
}

void APDWVehiclePawn::OnSteeringChanged(const FOnAttributeChangeData& Data)
{
	SetSteeringMultiplier(Data.NewValue);
}

void APDWVehiclePawn::OnAccelerationChanged(const FOnAttributeChangeData& Data)
{
	SetAccelerationMultiplier(Data.NewValue * 10);
}

void APDWVehiclePawn::OnTopSpeedChanged(const FOnAttributeChangeData& Data)
{
	// m/s to kmh
	SetMaxSpeed(Data.NewValue * 3.6f);
}

void APDWVehiclePawn::UpdateGravity(float DeltaTime)
{
	if (!bUseCustomGravity)	return;
	
	FVector GravityDirection = FVector::ZeroVector;
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		const FVector Normal = SuspensionComponents[i]->GetHitResult().ImpactNormal;
		/*float UpDot = Normal | FVector::UpVector;
		float Threshold = 0.6f;*/
		if (SuspensionComponents[i]->IsInAir())
		{
			GravityDirection += FVector::UpVector;
		}
		else
		{
			GravityDirection += Normal;
		}
	}
	GravityDirection = GravityDirection.GetSafeNormal();

	float GravityForce = GravityMultiplier * BodyMass * -BaseGravity;

	if (WasInAir && !bOnAir && ResetGravityOverTimeOnAirTimer <= 0.f)
	{
		AdditionalOnAirTimer = PreviousInAirTime;
		ResetGravityOverTimeOnAirTimer = ResetGravityOverTimeOnAirDelay;
	}

	if (ResetGravityOverTimeOnAirTimer > 0.f)
	{
		ResetGravityOverTimeOnAirTimer-= DeltaTime;
		AdditionalOnAirTimer += DeltaTime;
	}
	else if (!bOnAir)
	{
		AdditionalOnAirTimer = 0.f;
	}

	AdditionalOnAirTimer += AfterSpawnAirTime;
	//GEngine->AddOnScreenDebugMessage(1001, -1, FColor::Red, *FString::Printf(TEXT("InAir: %s,   OnAirTime: %f,   AdditionalOnAirTimer: &%f"), bOnAir ? "true" : "false", OnAirTime, AdditionalOnAirTimer));

	if (bOnAir && GravityOverTimeOnAir)
	{
		if (GetGroundDistance() * 0.01 > MinHeightForGravityOverTimeOnAir)
		{
			GravityForce *= GravityOverTimeOnAir->GetFloatValue(OnAirTime + AdditionalOnAirTimer);
		}
	}

	if (!SkipSwapGravity)
	{
		AddForce(GravityForce * GravityDirection, "Gravity");
	}
	SkipSwapGravity = false;
	AfterSpawnAirTime = 0.f;
}

void APDWVehiclePawn::CheckGround()
{
	for (int i = 0; i < 4; i++)
	{
		GroundDistance += SuspensionComponents[i]->GetGroundDistance();
	}
	GroundDistance /= 4;
	if (CurrentGroundMaterialName != GetWheelMaterial(0))
	{
		CurrentGroundMaterialName = GetWheelMaterial(0);
		OnGroundMaterialChanged.Broadcast(CurrentGroundMaterialName);
	}
}

void APDWVehiclePawn::UpdateSteering(float DeltaTime)
{
	if (!bSteerOnAir && bOnAir) return;

	const float AirMulti = bOnAir ? 0.6f : 1.f;
	const float MiniGameMulti = bInMiniGame ? MinigameSteeringMult : FMath::Sign(BodyLinearSpeed.X);
	AddTorque(FVector::UpVector * SteeringInput * AdditionalSteeringTorque * AirMulti * MiniGameMulti, true, "Steering");
}

void APDWVehiclePawn::UpdateAirStabilizer(float DeltaTime)
{
	if (!bUseAirStabilizer) return;

	if (OnAirWheel <= 2) return;
	const float PitchDot = BodyForward | FVector::UpVector;
	const float RollDot = BodyRight | FVector::UpVector;

	const float PitchDotToAim = !PitchToAimOnVerticalSpeed ? 0 : (PitchToAimOnVerticalSpeed->GetFloatValue(BodyLinearSpeed.Z * 3.6f) / 90);

	const FVector Torque = ((AirStabilizerPitchBaseForce * (FMath::Pow(abs(PitchDot - PitchDotToAim), 0.5f)) * FMath::Sign(PitchDot - PitchDotToAim) * BodyRight) -
		(AirStabilizerRollBaseForce * (RollForceMultiplierOverRoll ? RollForceMultiplierOverRoll->GetFloatValue(abs(RollDot)) : 1) * FMath::Sign(RollDot) * BodyForward));

	AddTorque(Torque, true, "AirStabilizer");
}

void APDWVehiclePawn::OnTakeOff()
{

}

void APDWVehiclePawn::OnLanding()
{

}

void APDWVehiclePawn::InitAttributes(const FGameplayTag& PupTag)
{
	if (!CurrentVehicleData) return;

	FPDWVehicleInfo* Data = &CurrentVehicleData->DefaultData;
	if (CurrentVehicleData->OverridePupData.Contains(PupTag))
	{
		Data = &CurrentVehicleData->OverridePupData[PupTag];
	}
	if (!Data) return;

	EngineTorque = Data->EngineForce;
	MaxSpeed = Data->MaxSpeedKPH;
	MaxSpeedInReverse = Data->MaxReverseSpeedKPH;
	OriginalMaxSpeed = MaxSpeed;

	AutoBrakeForce = Data->AutoBrakeForce;

	AdditionalSteeringTorque = Data->SteeringForce;
	CurveSteeringInputClamp = Data->SteeringOverVelocity;

	BaseGravity = Data->GravityForce;
	GravityOverTimeOnAir = Data->GravityOverTimeOnAir;

	AirStabilizerPitchBaseForce = Data->AirStabilizerPitchBaseForce;
	AirStabilizerRollBaseForce = Data->AirStabilizerRollBaseForce;

	MinigameSteeringMult = Data->MinigameSteeringMult;
	MinigameThrottleMult = Data->MinigameThrottleMult;

	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->SetStiffness(Data->SuspensionStiffness);
		SuspensionComponents[i]->SetDamping(Data->SuspensionDamping);
		SuspensionComponents[i]->SetExtensionLength(Data->SuspensionLenght);
	}

	InitialSuspensionLenght = Data->SuspensionLenght;
}

void APDWVehiclePawn::PhysicsGameTick(float DeltaTime)
{
	if (!bPhysicsEnabled) return;
	Super::PhysicsGameTick(DeltaTime);

	bCoyoteTimeActive = bOnAir && (OnAirTime < CoyoteTimeDuration) && bUseCoyoteTime;
	CheckGround();

	UE_VLOG(this, PDWVehicleLog, Verbose, TEXT("%f"), BodyLinearSpeed.X);
}

void APDWVehiclePawn::CustomizeVehicle(FGameplayTag CurrentPup)
{
	UNebulaGraphicsCustomizationComponent* GraphicsCustomizationComp = GetComponentByClass<UNebulaGraphicsCustomizationComponent>();
	if (GraphicsCustomizationComp)
	{
		FGameplayTagContainer ContainerTags;
		FCustomizationData CustomizationData = UPDWDataFunctionLibrary::GetCustomizationData(this);
		if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PDWPlayerController) && CustomizationData.GuestVehicleCustomizations.Contains(CurrentPup))
		{
			ContainerTags.AddTag(CustomizationData.GuestVehicleCustomizations[CurrentPup]);
		} 
		else if(CustomizationData.VehicleCustomizations.Contains(CurrentPup))
		{
			ContainerTags.AddTag(CustomizationData.VehicleCustomizations[CurrentPup]);
		}

		if (!ContainerTags.IsEmpty())
		{
			GraphicsCustomizationComp->CustomizeMaterial(ContainerTags);
		}
	}
}

void APDWVehiclePawn::ApplyVehicleDebuff(float SpeedMultiplier, bool bReset,float Timer /*= 0.0f*/,float AccelerationValue /*= 0*/)
{
	// #DEV <honestly vehicle class hould be still reworked, missing some basics stuff...> [#daniele.m, 7 October 2025, ApplyVehicleDebuff]
	if(!GetWorld()) return;

	const float SpeedToUse = SpeedMultiplier*OriginalMaxSpeed;
	SetMaxSpeed(SpeedToUse,0,bReset);
	const float ReverseSpeed = OriginalReverseSpeed*SpeedMultiplier;
	MaxSpeedInReverse = bReset ? OriginalReverseSpeed :ReverseSpeed;
	if (AccelerationValue!= 0)
	{
		SetAccelerationMultiplier(AccelerationValue);
	}
	GetWorld()->GetTimerManager().SetTimer(SpeedDebuffHandler, [=, this]()
		{
			float AccellerationToUse = 1.0f;
			if (bInMiniGame)
			{
				AccellerationToUse = MinigameThrottleMult;
			}
			SetAccelerationMultiplier(AccellerationToUse);
			ApplyVehicleDebuff(0,true);
			if (bBoosting)
			{
				bBoosting = false;
			}
		},Timer,false);
}

void APDWVehiclePawn::OverrideSuspensionsLenght(float NewSuspensionLenght)
{
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->SetExtensionLength(NewSuspensionLenght);
	}
}

void APDWVehiclePawn::SetCustomizationSuspensionsLenght()
{
	if (!CurrentVehicleData) return;

	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->SetExtensionLength(CurrentVehicleData->DefaultData.CustomizationSuspensionLenght);
	}
}

float APDWVehiclePawn::GetSuspensionsLenght()
{
	return  SuspensionComponents.Num() > 0 ? SuspensionComponents[0]->GetExtensionLenght() : 0.f;
}

void APDWVehiclePawn::ResetSuspensionsLenght()
{
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->SetExtensionLength(InitialSuspensionLenght);
	}
}

void APDWVehiclePawn::Look(const FVector2D& LookAxisVector)
{
	if (Controller != nullptr)
	{
		// TODO: aggiungere moltiplicatore degli assi
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APDWVehiclePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpeedDebuffHandler);
	}
	Super::EndPlay(EndPlayReason);
}

void APDWVehiclePawn::SetInitialVelocity(FVector inVelocity)
{
	InitialVelocity = inVelocity;
}

void APDWVehiclePawn::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = PDWPlayerController->GetPDWPlayerState()->GetPlayerStates();
}
