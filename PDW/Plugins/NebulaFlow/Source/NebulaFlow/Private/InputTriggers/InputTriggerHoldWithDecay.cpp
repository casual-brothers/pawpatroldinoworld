// Fill out your copyright notice in the Description page of Project Settings.

#include "InputTriggers/InputTriggerHoldWithDecay.h"
#include "EnhancedPlayerInput.h"

UInputTriggerHoldWithDecay::UInputTriggerHoldWithDecay()
{
	bShouldAlwaysTick = true;
}

ETriggerState UInputTriggerHoldWithDecay::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	ETriggerState State = ETriggerState::Ongoing;
	HeldDuration = CalculateHeldWithDecay(PlayerInput, DeltaTime, IsActuated(ModifiedValue));

	// Trigger when HeldDuration reaches the threshold
	bool bIsFirstTrigger = !bTriggered;
	bTriggered = HeldDuration >= HoldTimeThreshold;
	if (bTriggered)
	{
		if (bIsOneShot)
		{
			HeldDuration = 0.f;
		}
		return (bIsFirstTrigger || !bIsOneShot) ? ETriggerState::Triggered : ETriggerState::None;
	}
	
	return State;
}

float UInputTriggerHoldWithDecay::CalculateHeldWithDecay(const UEnhancedPlayerInput* const PlayerInput, const float DeltaTime, bool IsActuated)
{
	if (ensureMsgf(PlayerInput, TEXT("No Player Input was given to Calculate with! Returning HeldDuration")))
	{
		const float TimeDilation = PlayerInput->GetEffectiveTimeDilation();
	
		if (IsActuated)
		{
			DelayProgress = 0.f;
			float Result = HeldDuration + ((!bAffectedByTimeDilation ? DeltaTime : DeltaTime * TimeDilation) * (bGiveActuatedBoost ? ActuatedInputBoost : 1.f));
			bGiveActuatedBoost = false;
			return Result > HoldTimeThreshold ? HoldTimeThreshold : Result;
		}
		else
		{
			bGiveActuatedBoost = true;
			DelayProgress += DeltaTime;
			if (DelayProgress >= DecayDelay)
			{
				float Result = HeldDuration - (!bAffectedByTimeDilation ? (DeltaTime * DecayRate) : (DeltaTime * TimeDilation * DecayRate));
				return Result < 0.f ? 0.f : Result;
			}
		}
	}
	
	bGiveActuatedBoost = true;
	return HeldDuration;
}