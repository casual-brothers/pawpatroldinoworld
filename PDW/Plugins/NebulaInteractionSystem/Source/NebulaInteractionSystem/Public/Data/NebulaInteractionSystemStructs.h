#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "NebulaInteractionSystemStructs.generated.h"


USTRUCT(BlueprintType)
struct NEBULAINTERACTIONSYSTEM_API FInteractionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* Interactor = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	AActor* Interactable = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer InteractionTypes = FGameplayTagContainer();

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer InteractionTags = FGameplayTagContainer();

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer InteractionReceiverTags = FGameplayTagContainer();
};

USTRUCT(BlueprintType)
struct NEBULAINTERACTIONSYSTEM_API FBehaviourInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* InteractionBehaviourOwner = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AActor* Interactor = nullptr;

	//This property should be used to store any usefull tags that the behaviour might needs to use or check.
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer InfoTags = FGameplayTagContainer();
};

#pragma region EVENTS STRUCTS
USTRUCT(BlueprintType)
struct NEBULAINTERACTIONSYSTEM_API FInteractionBehaviourEventInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* InteractionBehaviourOwner = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AActor* Interactor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InteractionBehaviourID = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct NEBULAINTERACTIONSYSTEM_API FInteractionBehaviourInterruptInfo : public FInteractionBehaviourEventInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* InterruptionSource = nullptr;
};
#pragma endregion

UCLASS()
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionSystemStructs : public UObject
{
	GENERATED_BODY()
};