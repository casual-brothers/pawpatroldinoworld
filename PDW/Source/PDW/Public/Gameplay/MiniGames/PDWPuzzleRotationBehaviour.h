// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "PDWPuzzleRotationBehaviour.generated.h"

class UPDWPuzzlePieceComponent;
class UFMODEvent;
/**
 * 
 */
UCLASS()
class PDW_API UPDWPuzzleRotationBehaviour : public UPDWMovingObjectBehaviour
{
	GENERATED_BODY()
	
public:
	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;


	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;


	void RequestSceneComp(APDWPlayerController* inController) override;

protected:

	UPROPERTY(EditAnywhere,Category = "Config")
	float RotationValue = 90.0f;

	UPROPERTY(EditAnywhere,Category = "Config")
	UFMODEvent* RotateSoundEvent = nullptr;

	UPROPERTY()	
	UPDWPuzzlePieceComponent* PuzzlePiece = nullptr;
};
