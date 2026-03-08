// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PDWAbilitySpawnedActor.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FAbilityInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer SkillActionType;
};

UCLASS()
class PDW_API APDWAbilitySpawnedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWAbilitySpawnedActor();

	virtual void SendAbilityInfo(const FAbilityInfo& inAbilityInfo);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FAbilityInfo GetAbilityInfo();

protected:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USceneComponent* CustomRootComponent = nullptr;

	FAbilityInfo AbilityInfo;

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
