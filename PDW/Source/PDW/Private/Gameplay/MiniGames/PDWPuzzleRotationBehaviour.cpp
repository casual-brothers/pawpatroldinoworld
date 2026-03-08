// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWPuzzleRotationBehaviour.h"
#include "EnhancedInputComponent.h"
#include "Gameplay/Components/PDWPuzzlePieceComponent.h"
#include "Camera/CameraComponent.h"
#include "FMODBlueprintStatics.h"

void UPDWPuzzleRotationBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	const bool BoolValue = inInputInstance.GetValue().Get<bool>();
	//means i'm using axis -> so move input
	if (BoolValue)
	{
		if(!SceneComponentInstance)
			return;

		const FVector2D Value = inInputInstance.GetValue().Get<FVector2D>();
		const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();

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
			FVector NewDirection = Direction.RotateAngleAxis(CameraComp->GetRelativeRotation().Yaw, FVector::UpVector);
			SceneComponentInstance->AddRelativeLocation(NewDirection);
		}
	}
	else //means i'm pressing the single button so rotate
	{
		if (IsValid(PuzzlePiece))
		{
			UFMODBlueprintStatics::PlayEvent2D(PuzzlePiece->GetWorld(), RotateSoundEvent,true);
			PuzzlePiece->RotatePiece(RotationValue);
		}
	}
	UPDWMiniGameInputBehaviour::ExecuteBehaviour(inInputInstance);
}

void UPDWPuzzleRotationBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	Super::InitializeBehaviour(inController,inMiniGameComp);
	PuzzlePiece = Cast<UPDWPuzzlePieceComponent>(SceneComponentInstance);
}

void UPDWPuzzleRotationBehaviour::RequestSceneComp(APDWPlayerController* inController)
{
	Super::RequestSceneComp(inController);
	PuzzlePiece = Cast<UPDWPuzzlePieceComponent>(SceneComponentInstance);
}
