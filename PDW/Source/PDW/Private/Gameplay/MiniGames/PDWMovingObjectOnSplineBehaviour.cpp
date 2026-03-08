// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWMovingObjectOnSplineBehaviour.h"
#include "Components/SplineComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Gameplay/Actors/PDWSplineActor.h"
#include "Gameplay/Components/PDWPreyComponent.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Animation/PDWAnimInstance.h"

void UPDWMovingObjectOnSplineBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	int32 Index = -1;
	const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();
	for (const FGameplayTag& SplineTag : MovingSplineConfig->SplineTags)
	{
		USplineComponent* Spline = inMiniGameComp->GetOwner()->FindComponentByTag<USplineComponent>(SplineTag.GetTagName());
		if (!Spline)
		{
			continue;
		}
		SplineComponents.Add(Index,Spline);
		Index++;
	}

	USpringArmComponent* MinigameCamera = inMiniGameComp->GetOwner()->FindComponentByClass<USpringArmComponent>();
	if (MinigameCamera)
	{
		MinigameCameraComp = MinigameCamera;
	}

	CurrentSplineIndex = MovingSplineConfig->StartingSplineIndex;
	CachedSpeed = MovingSplineConfig->SpeedOnSpline;
	CachedTransitionDuration = MovingSplineConfig->TransitionToSplineTime;
	CachedPushDistance = MovingSplineConfig->PushbackDistance;
	CachedPushbackDuration = MovingSplineConfig->PushBackDuration;
	DeltaZOnSpline = MovingSplineConfig->DeltaZOnSpline;
	CameraSpeed = MovingSplineConfig->CameraSpeed;
	DefaultSplineIndex = MovingSplineConfig->DefaultSplineIndex;
	MinCameraDistanceFromPlayer = MovingSplineConfig->MinCameraDistanceFromPlayer;
	AcceptableCameraSpeedRange = MovingSplineConfig->AcceptableCameraSpeedRange;
	CameraDeltaSpeed = MovingSplineConfig->CameraDeltaSpeed;

	Super::InitializeBehaviour(inController,inMiniGameComp);

	TScriptInterface<IPDWPlayerInterface> PlayerInterface = GetPossesedPupInstance();
	if (PlayerInterface)
	{
		UPDWInteractionComponent* InteractionComp = PlayerInterface->GetInteractionComponent();
		InteractionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &UPDWMovingObjectOnSplineBehaviour::BeginOverlap);
		InteractionComp->OnComponentEndOverlap.AddUniqueDynamic(this, &UPDWMovingObjectOnSplineBehaviour::EndOverlap);
		if (MovingSplineConfig->PossesRule == EMiniGameActorSource::PossesPup)
		{
			if (PlayerInterface->GetPDWPlayerController()->GetVehicleInstance())
			{
				PlayerInterface->GetPDWPlayerController()->GetVehicleInstance()->SetPhysActiveCarBP(false);
			}
		}
	}

	if (MovingSplineConfig->bSpawnPrey && MovingSplineConfig->PreyClass)
	{
		APDWSplineActor* SplineActor = Cast<APDWSplineActor>(SplineComponents[0]->GetOwner());
		if (SplineActor)
		{
			FPDWSplineConfiguration SplineActorConfig;
			SplineActor->GetSplineConfiguration(SplineActorConfig);
			SplinesOffsetDistance = SplineActorConfig.DistanceBetweenSplines;
			Prey = SplineActor->SpawnPrey(MovingSplineConfig->PreyClass);
			if (Prey)
			{
				Prey->SetPlayerPawn(GetPossesedPupInstance());
			}
		}
	}
	UPDWEventSubsytem::Get(inController)->OnRamMode.AddUniqueDynamic(this, &UPDWMovingObjectOnSplineBehaviour::SetRamModeActive);
	UPDWEventSubsytem::Get(inController)->OnStopRamMode.AddUniqueDynamic(this, &UPDWMovingObjectOnSplineBehaviour::SetRamModeInactive);
	if (MovingSplineConfig->bAllowJumpEverywhere)
	{
		UPDWEventSubsytem::Get(inController)->OnJumpEventOnSpline.AddUniqueDynamic(this,&UPDWMovingObjectOnSplineBehaviour::Jump);
		UPDWEventSubsytem::Get(inController)->OnStopJumpEvent.AddUniqueDynamic(this,&UPDWMovingObjectOnSplineBehaviour::StopJump);
	}

	if (MovingSplineConfig->bOverridePupSpeed && MovingSplineConfig->MainMeshTag.IsValid())
	{
		USkeletalMeshComponent* Mesh = PossessedPupInstance->FindComponentByTag<USkeletalMeshComponent>(MovingSplineConfig->MainMeshTag.GetTagName());
		if (Mesh)
		{
			UPDWAnimInstance* AnimInstance =Cast<UPDWAnimInstance>(Mesh->GetAnimInstance());
			if (AnimInstance)
			{
				AnimInstance->SetSpeedOverride(MovingSplineConfig->SpeedOnSpline);
			}
		}
	}
}

void UPDWMovingObjectOnSplineBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	if (bIsTransitioning || bIsPushedBack || bStopLogic || bJumping)
	{
		return;
	}

	const FVector2D Value = inInputInstance.GetValue().Get<FVector2D>();
	
	if (Value.X == 0) 
	{
		return;
	}

	if (Value.X > 0 && CurrentSplineIndex == 1)
	{
		return;
	}
	if (Value.X < 0 && CurrentSplineIndex == -1)
	{
		return;
	}
	const int32 SelectedSplineIndex = (Value.X > 0 ? 1 : -1);
	const bool CanSwap = CheckSideBeforeSwitchLane(SelectedSplineIndex);
	if (!CanSwap)
	{
		// #TODO_PDW <Add audio feedback? some feedback needed?> [#daniele.m, 30 September 2025, ExecuteBehaviour]
		return;
	}
	int32 NewIndex = CurrentSplineIndex + SelectedSplineIndex;
	NewIndex = FMath::Clamp(NewIndex, -1, 1);
	bCameraShouldUpdate = true;
	//trace on sides to be sure to not change line when is occupied

	if (!SplineComponents.Contains(NewIndex))
	{
		return;
	}

	TScriptInterface<IPDWPlayerInterface> PlayerInterface = GetPossesedPupInstance();
	if (PlayerInterface && PlayerInterface->GetPDWPlayerController())
	{
		APDWVehiclePawn* Vehicle = PlayerInterface->GetPDWPlayerController()->GetVehicleInstance();
		if (Vehicle)
		{
			Vehicle->BP_OnTurnDuringMinigame(SelectedSplineIndex);
		}
	}
	// Prepare transition to same distance on new spline
	const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();
	bIsTransitioning = true;
	TransitionTimer = 0.0f;
	CachedTransitionDuration = MovingSplineConfig->TransitionToSplineTime;
	TargetSplineIndex = NewIndex;

	USplineComponent* CurrentSpline = SplineComponents[CurrentSplineIndex];
	USplineComponent* NextSpline = SplineComponents[TargetSplineIndex];

	TransitionStartLocation = GetPossesedPupInstance()->GetActorLocation();
	TransitionStartRotation = GetPossesedPupInstance()->GetActorRotation();

	//const FVector RightVector = TransitionStartRotation.RotateVector(FVector::RightVector);
	const FVector RightVector = GetPossesedPupInstance()->GetActorRightVector();
	const FVector ProjectionOnSpline = TransitionStartLocation + (RightVector * SelectedSplineIndex * SplinesOffsetDistance);

	// Project current location onto the next spline
	const FVector ClosestLocation = NextSpline->FindLocationClosestToWorldLocation(ProjectionOnSpline, ESplineCoordinateSpace::World);// project on the left or right and will fix the issue with crossroads
	const float ClosestDistance = NextSpline->GetDistanceAlongSplineAtLocation(ClosestLocation,ESplineCoordinateSpace::World) + CachedSpeed*CachedTransitionDuration;
	const FVector UpdatedClosestLocation = NextSpline->GetLocationAtDistanceAlongSpline(ClosestDistance,ESplineCoordinateSpace::World);

	TransitionEndLocation = UpdatedClosestLocation;
	TransitionEndRotation = NextSpline->GetRotationAtDistanceAlongSpline(ClosestDistance, ESplineCoordinateSpace::World);

	// Store new distance for continued movement after transition
	CurrentDistance = ClosestDistance;

	BP_ExecuteBehaviour_Implementation(inInputInstance);
}

void UPDWMovingObjectOnSplineBehaviour::CustomTick(const float inDeltaTime)
{
	if (!GetPossesedPupInstance())
	{
		return;
	}

	if (bRunCompleted)
	{
		return;
	}

	if (!bIsPushedBack && bCameraShouldUpdate)
	{
		UpdateCameraPosition(inDeltaTime);
	}

	if (bStopLogic)
	{
		return;
	}

	if (CachedObstacle)
	{
		const FVector2D PlayerCurrentPosition = FVector2D(GetPossesedPupInstance()->GetActorLocation());
		const FVector2D PlayerForward = FVector2D(GetPossesedPupInstance()->GetActorForwardVector());
		const FVector2D ObstaclePosition = FVector2D(CachedObstacle->GetActorLocation());

		
		const FVector2D PlayerToObstacle = ObstaclePosition - PlayerCurrentPosition;
		const float DotProduct = FVector2D::DotProduct(PlayerForward.GetSafeNormal(),PlayerToObstacle.GetSafeNormal());
		if (DotProduct < 0.0f)
		{
			bCameraShouldUpdate = true;
			CachedObstacle = nullptr;
		}
	}


	// Handle spline transition
	if (bIsTransitioning)
	{
		TransitionTimer += inDeltaTime;
		float Alpha = FMath::Clamp(TransitionTimer / CachedTransitionDuration, 0.0f, 1.0f);
		//TransitionEndLocation.Z = TransitionStartLocation.Z;
		FVector NewLocation = FMath::Lerp(TransitionStartLocation, TransitionEndLocation, Alpha);
		const FRotator NewRotation = FMath::Lerp(TransitionStartRotation, TransitionEndRotation, Alpha);
		GetPossesedPupInstance()->SetActorLocationAndRotation(NewLocation, FQuat::MakeFromRotator(NewRotation));

		if (Alpha >= 1.0f)
		{
			bIsTransitioning = false;
			CurrentSplineIndex = TargetSplineIndex;
		}
		return;
	}

	// Handle pushback
	if (bIsPushedBack)
	{
		PushbackTimer += inDeltaTime;
		float Alpha = FMath::Clamp(PushbackTimer / CachedPushbackDuration, 0.0f, 1.0f);

		float InterpDistance = FMath::Lerp(PushbackStartDistance,PushbackTargetDistance, Alpha);
		CurrentDistance = InterpDistance;

		if (SplineComponents.Contains(CurrentSplineIndex))
		{
			const FVector NewLocation = SplineComponents[CurrentSplineIndex]->GetWorldLocationAtDistanceAlongSpline(CurrentDistance) + FVector::UpVector*JumpValue;
			const FRotator NewRotation = SplineComponents[CurrentSplineIndex]->GetWorldRotationAtDistanceAlongSpline(CurrentDistance);
			GetPossesedPupInstance()->SetActorLocationAndRotation(NewLocation, FQuat::MakeFromRotator(NewRotation));
		}

		if (Alpha >= 1.0f)
		{
			bIsPushedBack = false;
		}
		return;
	}

	// Normal movement along spline
	if (!SplineComponents.Contains(CurrentSplineIndex))
	{
		return;
	}

	CurrentDistance += inDeltaTime * CachedSpeed;
	const float SplineLength = SplineComponents[CurrentSplineIndex]->GetSplineLength();

	if (CurrentDistance < SplineLength)
	{
		FVector NewLocation = SplineComponents[CurrentSplineIndex]->GetWorldLocationAtDistanceAlongSpline(CurrentDistance) + FVector::UpVector*JumpValue;
		NewLocation.Z += DeltaZOnSpline;
		const FRotator NewRotation = SplineComponents[CurrentSplineIndex]->GetWorldRotationAtDistanceAlongSpline(CurrentDistance);
		GetPossesedPupInstance()->SetActorLocationAndRotation(NewLocation, FQuat::MakeFromRotator(NewRotation));
	}
	else
	{
		const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();
		bRunCompleted = true;
		FInteractionEventSignature InteractionEvent;
		InteractionEvent.EventTag = MovingSplineConfig->EventID;
		InteractionEvent.Interacter = GetControllerOwner();
		IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(GetPossesedPupInstance());
		if (PlayerInterface)
		{
			UPDWInteractionComponent* InteractionComp = PlayerInterface->GetInteractionComponent();
			InteractionComp->OnComponentBeginOverlap.RemoveDynamic(this, &UPDWMovingObjectOnSplineBehaviour::BeginOverlap);
			if (MovingSplineConfig->PossesRule != EMiniGameActorSource::PossesSpecificClass)
			{
				if (PlayerInterface->GetPDWPlayerController()->GetIsOnVehicle())
				{
					PlayerInterface->GetPDWPlayerController()->GetVehicleInstance()->SetPhysActiveCarBP(true);
					PlayerInterface->GetPDWPlayerController()->ChangeVehicle();
				}
			}
		}
		UPDWEventSubsytem::Get(GetControllerOwner())->TriggerInteractionEvent(MovingSplineConfig->TargetIDToNotify, InteractionEvent);
	}
}

void UPDWMovingObjectOnSplineBehaviour::UpdateCameraPosition(const float inDeltaTime)
{
	if (MinigameCameraComp && GetPossesedPupInstance())
	{
		
		const FVector2D CameraCurrentPosition = FVector2D(MinigameCameraComp->GetComponentLocation());
		const FVector2D PlayerCurrentPosition = FVector2D(GetPossesedPupInstance()->GetActorLocation());
		const FVector2D PlayerForward = FVector2D(GetPossesedPupInstance()->GetActorForwardVector());

		//DrawDebugSphere(GetWorld(),MinigameCameraComp->GetComponentLocation(), 30, 32, FColor::Green,false,5);
		//DrawDebugSphere(GetWorld(),GetPossesedPupInstance()->GetActorLocation(), 30, 32, FColor::Purple,false,5);

		const FVector2D PlayerToCamera = CameraCurrentPosition - PlayerCurrentPosition;

		const float Distance = PlayerToCamera.Size();
		const float DotProduct = FVector2D::DotProduct(PlayerForward.GetSafeNormal(),PlayerToCamera.GetSafeNormal());
		float CameraSpeedToUse = CameraSpeed;
		if (Distance - CameraSpeed*inDeltaTime >= MinCameraDistanceFromPlayer && DotProduct < 0.0f)
		{
			if (Distance < AcceptableCameraSpeedRange.GetLowerBoundValue())
			{
				CameraSpeedToUse = CameraSpeed - CameraDeltaSpeed;
			}
			else if (Distance > AcceptableCameraSpeedRange.GetUpperBoundValue())
			{
				CameraSpeedToUse = CameraSpeed + CameraDeltaSpeed;
			}
			else
			{
				CameraSpeedToUse = CameraSpeed;
			}
			CurrentCameraPosition += inDeltaTime * CameraSpeedToUse;
			const FVector NewLocation = SplineComponents[DefaultSplineIndex]->GetWorldLocationAtDistanceAlongSpline(CurrentCameraPosition);
			const FRotator NewRotation = SplineComponents[DefaultSplineIndex]->GetWorldRotationAtDistanceAlongSpline(CurrentCameraPosition);
			FTransform NewTransform;
			NewTransform.SetLocation(NewLocation);
			NewTransform.SetRotation(FQuat::MakeFromRotator(NewRotation));
			MinigameCameraComp->SetWorldTransform(NewTransform);
		}

		//GEngine->AddOnScreenDebugMessage(1000, -1, FColor::Red, *FString::Printf(TEXT("Speed: %f"), CameraSpeedToUse));
	}
}

void UPDWMovingObjectOnSplineBehaviour::UninitializeBehaviour()
{
	ResetVariables();
	if (Prey)
	{
		Prey->GetOwner()->Destroy();
	}
	APawn* PossessedPup = GetPossesedPupInstance();
	if (IsValid(PossessedPup))
	{
		IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(PossessedPup);
		if (PlayerInterface)
		{
			UPDWInteractionComponent* InteractionComp = PlayerInterface->GetInteractionComponent();
			InteractionComp->OnComponentBeginOverlap.RemoveDynamic(this, &UPDWMovingObjectOnSplineBehaviour::BeginOverlap);
			const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();
			if (MovingSplineConfig->PossesRule != EMiniGameActorSource::PossesSpecificClass)
			{
				if (PlayerInterface->GetPDWPlayerController()->GetIsOnVehicle())
				{
					PlayerInterface->GetPDWPlayerController()->GetVehicleInstance()->SetPhysActiveCarBP(true);
					PlayerInterface->GetPDWPlayerController()->ChangeVehicle();
				}
			}
		}
	}
	if (IsValid(Owner))
	{
		UPDWEventSubsytem::Get(Owner)->OnJumpEventOnSpline.RemoveAll(this);
		UPDWEventSubsytem::Get(Owner)->OnStopJumpEvent.RemoveAll(this);
		UPDWEventSubsytem::Get(Owner)->OnRamMode.RemoveAll(this);
		UPDWEventSubsytem::Get(Owner)->OnStopRamMode.RemoveAll(this);
	}
	Super::UninitializeBehaviour();
}

void UPDWMovingObjectOnSplineBehaviour::ResetVariables()
{
	// #DEV <add here all variables to reset> [#daniele.m, 30 October 2025, ResetVariables]
	//bRunCompleted = true;
	CurrentCameraPosition = 0;
	CurrentDistance = 0;
	bIsTransitioning = false;
	bIsPushedBack = false;
	bStopLogic = false;
	bJumping = false;
	bWaitForReset = false;	
	bCameraShouldUpdate = true;
	bRamModeActive = false;
	if (MinigameCameraComp)
	{
		MinigameCameraComp->SetRelativeLocation(FVector::ZeroVector);
	}
}

void UPDWMovingObjectOnSplineBehaviour::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();
	if (OtherComp->GetCollisionProfileName() == MovingSplineConfig->ObstacleCollisionProfile.Name)
	{
		if (bRamModeActive)
		{
			if (OtherComp->ComponentTags.Contains(FName("Destructible")))
			{
				return;
			}
		}

		if (bIsTransitioning || bIsPushedBack)
		{
			return;
		}
	
		const FVector2D PlayerCurrentPosition = FVector2D(GetPossesedPupInstance()->GetActorLocation());
		const FVector2D PlayerForward = FVector2D(GetPossesedPupInstance()->GetActorForwardVector());
		const FVector2D ObstaclePosition = FVector2D(OtherComp->GetOwner()->GetActorLocation());

		
		const FVector2D PlayerToObstacle = ObstaclePosition - PlayerCurrentPosition;
		const float DotProduct = FVector2D::DotProduct(PlayerForward.GetSafeNormal(),PlayerToObstacle.GetSafeNormal());
		if (DotProduct > 0.0f)
		{
			bIsPushedBack = true;
			PushbackTimer = 0.0f;
			bCameraShouldUpdate = false;
			CachedObstacle = OtherComp->GetOwner();
			PushbackStartDistance = CurrentDistance;
			PushbackTargetDistance = FMath::Max(CurrentDistance - CachedPushDistance, 0.0f); // Prevent negative distance
			BP_BeginOverlap_Implementation(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			if (PossessedPupInstance && PossessedPupInstance->GetController())
			{
				UPDWGameplayFunctionLibrary::PlayForceFeedback({GetControllerOwner()}, UPDWGameplayTagSettings::GetVehicleImpactVibrationTag());	
			}
		}
	}
	else if (OtherComp->GetCollisionProfileName() == MovingSplineConfig->RampCollisionProfile.Name)
	{
		if (OtherComp->ComponentTags.Contains("Enter"))
		{
			//GEngine->AddOnScreenDebugMessage(14000, 1, FColor::Red, *FString::Printf(TEXT("StopLogic true:")));
			
			bStopLogic = true;
		}

		if (OtherComp->ComponentTags.Contains("Exit"))
		{
			CurrentDistance = SplineComponents[CurrentSplineIndex]->GetDistanceAlongSplineAtLocation(GetPossesedPupInstance()->GetActorLocation(),ESplineCoordinateSpace::World);
			bStopLogic = false;
			//GEngine->AddOnScreenDebugMessage(10200, 1, FColor::Red, *FString::Printf(TEXT("StopLogic false:")));

		}

	}
}

void UPDWMovingObjectOnSplineBehaviour::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();


	if (OtherComp->GetCollisionProfileName() == MovingSplineConfig->ObstacleCollisionProfile.Name)
	{
		const FVector2D PlayerCurrentPosition = FVector2D(GetPossesedPupInstance()->GetActorLocation());
		const FVector2D PlayerForward = FVector2D(GetPossesedPupInstance()->GetActorForwardVector());
		const FVector2D ObstaclePosition = FVector2D(OtherComp->GetOwner()->GetActorLocation());

		
		const FVector2D PlayerToObstacle = ObstaclePosition - PlayerCurrentPosition;
		const float DotProduct = FVector2D::DotProduct(PlayerForward.GetSafeNormal(),PlayerToObstacle.GetSafeNormal());
		if (DotProduct < 0.0f)
		{
			bCameraShouldUpdate = true;
		}
	}
}

void UPDWMovingObjectOnSplineBehaviour::Jump(const float& inJumpValue)
{
	bJumping = true;
	
	if (!bIsTransitioning)
	{
		if (!bWaitForReset)
		{
			JumpValue = inJumpValue;
		}
	}
	else
	{
		bWaitForReset = true;
	}
}

void UPDWMovingObjectOnSplineBehaviour::StopJump()
{
	JumpValue = 0;
	bJumping = false;
	bWaitForReset = false;
}

bool UPDWMovingObjectOnSplineBehaviour::CheckSideBeforeSwitchLane(const int32& inSelectedIndex)
{
	const FMovingObjectOnSplineConfig* MovingSplineConfig = MovingObjectBehaviourConfig.GetPtr<FMovingObjectOnSplineConfig>();
	FHitResult Hit;
	const FVector StartPoint= GetPossesedPupInstance()->GetActorLocation() + GetPossesedPupInstance()->GetActorRightVector() * inSelectedIndex * MovingSplineConfig->TraceBoxHalfSize;
	return !UKismetSystemLibrary::BoxTraceSingleForObjects(this,StartPoint,StartPoint,MovingSplineConfig->BoxSize,GetPossesedPupInstance()->GetActorRotation(), MovingSplineConfig->TraceObjectTypes, false, {GetPossesedPupInstance()}, MovingSplineConfig->bDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green);
}

void UPDWMovingObjectOnSplineBehaviour::SetRamModeActive()
{
	bRamModeActive = true;
}

void UPDWMovingObjectOnSplineBehaviour::SetRamModeInactive()
{
	bRamModeActive = false;
}
