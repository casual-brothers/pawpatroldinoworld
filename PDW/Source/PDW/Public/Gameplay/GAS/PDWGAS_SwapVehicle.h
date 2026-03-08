// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GAS/PDWGAS_BaseSwapAbility.h"
#include "PDWGAS_SwapVehicle.generated.h"

class APDWVehiclePawn;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PDW_API UPDWGAS_SwapVehicle : public UPDWGAS_BaseSwapAbility
{
	GENERATED_BODY()

public: 

	UPROPERTY(EditAnywhere)
	FGameplayTag MeshToAttachTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag SocketToUse = FGameplayTag::EmptyTag;

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void FindNewLocation(FTransform& Location);

};
