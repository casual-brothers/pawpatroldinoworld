// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "Managers/PDWEventSubsytem.h"
#include "PDWJumpOnSplineBehaviour.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FJumpOnSplineConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UCurveFloat* JumpingCurve = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float JumpingMultiplier = 10.0f;

};

/**
 * 
 */
UCLASS()
class PDW_API UPDWJumpOnSplineBehaviour : public UPDWMiniGameInputBehaviour
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Configuration")
	FJumpOnSplineConfig JumpConfig;

	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;


	void CustomTick(const float inDeltaTime) override;


	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;

protected:

	UPROPERTY()
	UPDWEventSubsytem* EventSubsystem = nullptr;

	bool bCanJump = true;
	bool bJumping = false;
	float Timer = 0.0f;
	float MaxTimer = 0.0f;
};
