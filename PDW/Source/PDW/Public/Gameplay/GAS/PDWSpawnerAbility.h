// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "PDWSpawnerAbility.generated.h"

class APDWAbilitySpawnedActor;

UENUM(BlueprintType)
enum class ESpawnType : uint8
{
	SingleSpawn =		0,
	MultipleSpawn =		1,
};

USTRUCT(BlueprintType)
struct PDW_API FSpawningConfiguration
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<APDWAbilitySpawnedActor> ActorToSpawn = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ESpawnType SpawnType = ESpawnType::SingleSpawn;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "SpawnType == ESpawnType::MultipleSpawn",EditConditionHides))
	float SpawnInterval = 0.2f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "SpawnType == ESpawnType::MultipleSpawn",EditConditionHides))
	int32 SpawnAmount = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName SpawnSocket = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTag MeshToAttachTag = FGameplayTag::EmptyTag;

	//Add here extra info if are needed
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWSpawnerAbility : public UPDWGASAbility
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSpawningConfiguration SpawnConfiguration;

	virtual void SpawnActor(FActorSpawnParameters inParams,FTransform inTransform);
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void TriggerAbilityLogic_Implementation() override;

	UPROPERTY(BlueprintReadWrite)
	APDWAbilitySpawnedActor* SpawnedActor = nullptr;
private:
	
	int32 SpawnedActorsIndex = 0;
	FTimerHandle SpawnTimerHandler;

	void RequestSpawn();

};
