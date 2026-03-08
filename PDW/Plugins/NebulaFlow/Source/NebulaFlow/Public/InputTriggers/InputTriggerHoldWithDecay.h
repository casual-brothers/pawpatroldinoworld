// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputTriggers.h"
#include "InputTriggerHoldWithDecay.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Hold With Decay"))
class NEBULAFLOW_API UInputTriggerHoldWithDecay : public UInputTriggerTimedBase
{
	GENERATED_BODY()
	
	UInputTriggerHoldWithDecay();
	
protected:
	/*If set to 0, the input will not decay*/
	UPROPERTY(EditInstanceOnly, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
	float DecayRate = 0.5f;
	
	/*Time before decay starts after key release*/
	UPROPERTY(EditInstanceOnly, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
	float DecayDelay = 0.5f;
	
	/*Adds a fill boost on first key press*/
	UPROPERTY(EditInstanceOnly, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
	float ActuatedInputBoost = 20.f;

	UPROPERTY()
	float DelayProgress = 0.f;

	bool bGiveActuatedBoost = true;
	bool bTriggered = false;

	// How long does the input have to be held to cause trigger?
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
	float HoldTimeThreshold = 1.0f;

	// Should this trigger fire only once, or fire every frame once the hold time threshold is met?
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "Trigger Settings")
	bool bIsOneShot = false;

	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;

	float CalculateHeldWithDecay(const UEnhancedPlayerInput* const PlayerInput, const float DeltaTime, bool IsActuated);

public:

	const float GetProgression() const { return HeldDuration; };

	const float GetHoldTimeThreshold() const { return HoldTimeThreshold; };

	const bool GetIsOneShot() const { return bIsOneShot; };

	ETriggerEventsSupported GetSupportedTriggerEvents() const override {return ETriggerEventsSupported::Ongoing;};
};
