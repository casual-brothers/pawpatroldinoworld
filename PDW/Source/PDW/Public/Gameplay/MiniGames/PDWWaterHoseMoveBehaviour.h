// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "PDWWaterHoseMoveBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWWaterHoseMoveBehaviour : public UPDWMovingObjectBehaviour
{
	GENERATED_BODY()
	
public:
	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;


	void UninitializeBehaviour() override;

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Config")
	FName SocketToAttach = NAME_None;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Config")
	FGameplayTag MeshTag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Config")
	float MaxAngle = 35.0f;

	void InitializeSceneComponentPosses(APDWPlayerController* inController) override;

private:

	float YawDelta  = 0.0f;
	float PitchDelta = 0.0f;
};
