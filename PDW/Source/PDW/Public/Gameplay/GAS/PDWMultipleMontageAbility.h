// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDWSpawnerAbility.h"
#include "Gameplay/Interaction/PDWRayTraceBehaviour.h"
#include "PDWMultipleMontageAbility.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMultipleMontageAbility : public UPDWSpawnerAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	FTraceConfig Trace;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	float MinHeight = 30.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	FGameplayTag DefaulMontageSettingsTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration",meta =(ForceInlineRow))
	TMap<FGameplayTag,FMontageSettings> ExtraMontages = {};

	void OverrideMontageToPlaySettings() override;

	UPROPERTY(BlueprintReadWrite)
	bool bSkip = false;
public:
	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

};
