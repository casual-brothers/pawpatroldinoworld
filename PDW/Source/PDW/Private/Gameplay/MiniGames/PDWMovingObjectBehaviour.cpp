// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "EnhancedInputComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/Components/PDWMinigameMovingMesh.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Actors/PDWMinigame_Base.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWMovingObjectBehaviour::InitializeSceneComponentPosses(APDWPlayerController* inController)
{
	MiniGame = Cast<APDWMinigame_Base>(MiniGameOwner->GetOwner());
	if (MiniGame)
	{
		SceneComponentInstance = MiniGame->RequestMovingMeshComponent(inController);
		ensureMsgf(SceneComponentInstance != nullptr, TEXT("UPDWMovingObjectBehaviour::InitializeBehaviour - SceneComponentInstance is null. Please check the ObjectToPossesTag is set correctly in the MiniGameConfigComponent."));
		if (SceneComponentInstance)
		{
			SceneComponentInstance->PossesComponent(inController,this);
		}
	}
}

void UPDWMovingObjectBehaviour::InitializeBehaviour(APDWPlayerController* inController,UPDWMinigameConfigComponent* inMiniGameComp)
{
	Super::InitializeBehaviour(inController,inMiniGameComp);
	CameraComp = inMiniGameComp->GetOwner()->FindComponentByClass<UCameraComponent>();
	const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();
	if (Config->PossesRule == EMiniGameActorSource::PossesSceneComponent)
	{
		InitializeSceneComponentPosses(inController);
	}
	else if(Config->PossesRule == EMiniGameActorSource::PossesPup)
	{
		if (!inController->GetIsOnVehicle())
		{
			PossessedPupInstance = inController->GetPawn();
		}
		else
		{
			PossessedPupInstance = inController->GetVehicleInstance();
		}
	}
	else if (Config->PossesRule == EMiniGameActorSource::PossesSpecificClass)
	{
		if (Config->SpecificClassToPosses)
		{
			APawn* SpawnedPawn = Cast<APawn>(UGameplayStatics::GetActorOfClass(inController,Config->SpecificClassToPosses));
			PossessedPupInstance = SpawnedPawn;
		}
	}

	UPDWEventSubsytem::Get(this)->OnNotifyBind.AddUniqueDynamic(this,&UPDWMovingObjectBehaviour::OnExternalBindEvent);
}

void UPDWMovingObjectBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	const FVector2D Value = inInputInstance.GetValue().Get<FVector2D>();
	const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();
	if (Config->PossesRule == EMiniGameActorSource::PossesSceneComponent)
	{
		if(!SceneComponentInstance)
			return;

		FVector Direction = FVector::ZeroVector;
		if (Value.X != 0.0f)
		{
			Direction.Y = Value.X * Config->Speed * GetWorld()->GetDeltaSeconds();
		}
		if (Value.Y != 0.0f)
		{
			if (Config->MovingRules == EMovingPlane::XY)
			{
				Direction.X = Value.Y * Config->Speed * GetWorld()->GetDeltaSeconds();
			}
			else if (Config->MovingRules == EMovingPlane::XZ)
			{
				
				Direction.Z = Value.Y * Config->Speed * GetWorld()->GetDeltaSeconds();
			}
		}
		if (Direction.SizeSquared() > 0.0f)
		{
			FVector DirectionVector = (Config->MovingRules == EMovingPlane::XY) ?  FVector::UpVector : FVector::RightVector;
			//FVector NewDirection = Direction.RotateAngleAxis(CameraComp->GetRelativeRotation().Yaw, DirectionVector);
			FVector NewDirection = Direction.RotateAngleAxis(SceneComponentInstance->GetRelativeRotation().Yaw, FVector::UpVector);
			SceneComponentInstance->AddRelativeLocation(NewDirection);
			//GEngine->AddOnScreenDebugMessage(25658,5.0f,FColor::Green,FString::Printf(TEXT("NewDirection: %s"), *SceneComponentInstance->GetRelativeLocation().ToString()));
			
			/*float RelativePositionX = SceneComponentInstance->GetRelativeLocation().X;
			RelativePositionX = FMath::Clamp(RelativePositionX, Config->MinMaxSceneMovement.GetLowerBoundValue(), Config->MinMaxSceneMovement.GetUpperBoundValue());*/
			

			if (SceneComponentInstance->bClampMovingValue)
			{
				float RelativePositionY = SceneComponentInstance->GetRelativeLocation().Y;
				RelativePositionY = FMath::Clamp(RelativePositionY, SceneComponentInstance->MinMaxHorizontalMovement.GetLowerBoundValue(), SceneComponentInstance->MinMaxHorizontalMovement.GetUpperBoundValue());
				float RelativePositionZ = SceneComponentInstance->GetRelativeLocation().Z;
				RelativePositionZ = FMath::Clamp(RelativePositionZ, SceneComponentInstance->MinMaxVerticalMovement.GetLowerBoundValue(), SceneComponentInstance->MinMaxVerticalMovement.GetUpperBoundValue());
			
				FVector ClampedLocation = SceneComponentInstance->GetRelativeLocation();
				SceneComponentInstance->SetRelativeLocation(FVector(ClampedLocation.X,RelativePositionY,RelativePositionZ));

			}

		}

		UPDWEventSubsytem::Get(this)->OnMovingMeshBehaviourEvent(SceneComponentInstance);

	}
	else
	{
		const int32 Inv = CameraComp->GetComponentRotation().Yaw < 0 ? -1 : 1;
		FVector Forward =  (CameraComp->GetForwardVector() + CameraComp->GetUpVector());
		Forward.Normalize();
		const FVector Right = CameraComp->GetRightVector();
		if (GetControllerOwner())
		{
			GetControllerOwner()->MiniGameMove(Value * Inv, Forward * Inv, Right * Inv);
		}
	}

	Super::ExecuteBehaviour(inInputInstance);
}

void UPDWMovingObjectBehaviour::UninitializeBehaviour()
{
	if (SceneComponentInstance)
	{
		SceneComponentInstance->UnpossesComponent(true);
	}
	PossessedPupInstance = nullptr;
	CameraComp = nullptr;
	SceneComponentInstance = nullptr;
	if (GetControllerOwner())
	{
		GetControllerOwner()->MiniGameEnd();
	}
	UPDWEventSubsytem::Get(this)->OnNotifyBind.RemoveDynamic(this,&UPDWMovingObjectBehaviour::OnExternalBindEvent);
	Super::UninitializeBehaviour();
}

void UPDWMovingObjectBehaviour::RequestSceneComp(APDWPlayerController* inController)
{	
	const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();
	if (MiniGame && Config->bMultiplePosses)
	{
		UPDWMinigameMovingMesh* OldSceneComponentInstance = SceneComponentInstance;
		SceneComponentInstance = MiniGame->RequestMovingMeshComponent(inController);
		ensureMsgf(SceneComponentInstance != nullptr, TEXT("UPDWMovingObjectBehaviour::InitializeBehaviour - SceneComponentInstance is null. Please check the ObjectToPossesTag is set correctly in the MiniGameConfigComponent."));
		if (SceneComponentInstance)
		{
			SceneComponentInstance->PossesComponent(Owner,this);
			OnObjectChange.Broadcast(OldSceneComponentInstance, SceneComponentInstance);
		}
	}
}

UPDWMinigameMovingMesh* UPDWMovingObjectBehaviour::GetMovingMeshComponent()
{
	//ensureMsgf(SceneComponentInstance,TEXT("UPDWMovingObjectBehaviour - NoSceneComponent"));
	return SceneComponentInstance;
}

void UPDWMovingObjectBehaviour::SetPossesedPupInstance(APawn* inPup)
{
	PossessedPupInstance = inPup;
}

APawn* UPDWMovingObjectBehaviour::GetPossesedPupInstance()
{
	//ensureMsgf(PossessedPupInstance,TEXT("UPDWMovingObjectBehaviour - NoPossessedPupInstance"));
	return PossessedPupInstance;
}

UCameraComponent* UPDWMovingObjectBehaviour::GetCameraComponent()
{
	ensureMsgf(CameraComp,TEXT("UPDWMovingObjectBehaviour - NoCameraComp"));
	return CameraComp;
}

void UPDWMovingObjectBehaviour::OnExternalBindEvent(const FGameplayTag& ActionToBind, APDWPlayerController* InOwner)
{
	if (GetControllerOwner() == InOwner)
	{
		UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
		for (FInstancedStruct InstStruct : InputActionsInfo)
		{
			if (InstStruct.GetPtr<FInputActionInfo>()->InputActionsToUse == ActionToBind)
			{
				const UInputAction* InputAction = UPDWGameSettings::GetInputActionByTag(ActionToBind);
				for (const auto& Mode : InstStruct.GetPtr<FInputActionInfo>()->InputMode)
				{
					Input->BindAction(InputAction, Mode, this, &UPDWMovingObjectBehaviour::ExecuteBehaviour);
				}
			}
		}
	}
}

const EMiniGameActorSource UPDWMovingObjectBehaviour::GetMinigamePossesRule()
{
	EMiniGameActorSource Result = EMiniGameActorSource::PossesSpecificClass;
	const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();
	if (Config)
	{
		Result = Config->PossesRule;
	}

	return Result;
}

const bool UPDWMovingObjectBehaviour::HaveMinigameMultiplePosses()
{
	bool Result = false;
	const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();
	if (Config)
	{
		Result = Config->bMultiplePosses;
	}

	return Result;
}