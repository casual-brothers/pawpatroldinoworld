// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "PDWInteractAbility.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWInteractAbility : public UPDWGASAbility
{
	GENERATED_BODY()
	
	UPDWInteractAbility(const FObjectInitializer& ObjectInitializer);

public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
