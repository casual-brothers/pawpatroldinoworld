// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Components/PDWTargetingComponent.h"
#include "PDWDrillExcavationTargetingComp.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent),Blueprintable)
class PDW_API UPDWDrillExcavationTargetingComp : public UPDWTargetingComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<AActor*> BlockingRocks;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> RemovedRocks;

	UPROPERTY()
	TArray<AActor*> CachedBlockingRocks;
public:
	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type Reason) override;

	UFUNCTION(BlueprintCallable)
	void Reset();

protected:

	UFUNCTION()	
	void OnRockRemovedEvent(AActor* inRock);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRockRemoved(AActor* inRock);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnAllRockRemoved();
};
