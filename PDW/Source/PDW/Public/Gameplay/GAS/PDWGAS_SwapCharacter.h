// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GAS/PDWGAS_BaseSwapAbility.h"
#include "Camera/PlayerCameraManager.h"
#include "PDWGAS_SwapCharacter.generated.h"



/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PDW_API UPDWGAS_SwapCharacter : public UPDWGAS_BaseSwapAbility
{
	GENERATED_BODY()
	
protected:

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnSwapCompleted(APDWPlayerController* inController);
};
