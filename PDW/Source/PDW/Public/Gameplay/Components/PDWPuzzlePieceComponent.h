// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Components/PDWMinigameMovingMesh.h"
#include "PDWPuzzlePieceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrientationChanged,UPDWPuzzlePieceComponent*, Comp);

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PDW_API UPDWPuzzlePieceComponent : public UPDWMinigameMovingMesh
{
	GENERATED_BODY()
	
public:

	FOnOrientationChanged OnOrientationChanged;

	void UnpossesComponent(bool bUnInitializingBehaviour = false,APDWPlayerController* inController = nullptr) override;
	void RotatePiece(float inValue);
	void PossesComponent(APDWPlayerController* inController, UPDWMovingObjectBehaviour* MovingBehaviour = nullptr) override;

};
