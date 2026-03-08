// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PDWInteractionReceiverConfigData.generated.h"

class UPDWConditionCheck;
class UPDWInteractionBehaviour;
class UPDWOverlapBehaviour;

UENUM(BlueprintType,meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EInteractionCheckConfig : uint8
{
	None =					0<<0,
	WorkOnClosest =			1<<0,
};
ENUM_CLASS_FLAGS(EInteractionCheckConfig);

USTRUCT(BlueprintType)
struct PDW_API FPDWBehaviorMappingConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 BehaviorPriority = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //TODO -> Rename with SuccessConditionBehaviour
	FGameplayTag BehaviourIdentifier = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag FailedConditionBehaviour = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Instanced)
	TArray<UPDWConditionCheck*> BehaviourConditions {};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWBehaviorStateMappingConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForceInlineRow))
	TMap<FGameplayTag, FPDWBehaviorMappingConfig> StateBehaviourMapping;
};

UCLASS(Blueprintable)
class PDW_API UPDWInteractionReceiverConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag StartingState = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EInteractionCheckConfig))
	uint8 InteractionConfiguration = (uint8)(EInteractionCheckConfig::WorkOnClosest);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AllowedInteractions;
		
	UPROPERTY(EditAnywhere, meta = (ForceInlineRow), Instanced)
	TMap<FGameplayTag, TObjectPtr<UPDWInteractionBehaviour>> Behaviours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForceInlineRow))
	TMap<FGameplayTag, FPDWBehaviorStateMappingConfig> InteractionBehaviourMapping;

	UPROPERTY(EditAnywhere, meta = (ForceInlineRow), Instanced)
	TMap<FGameplayTag,TObjectPtr<UPDWOverlapBehaviour>> OverlapBehaviours;
};
