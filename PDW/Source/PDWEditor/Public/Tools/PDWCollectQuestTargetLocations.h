// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityActor.h"
#include "ActorActionUtility.h"
#include "PDWCollectQuestTargetLocations.generated.h"

class UFlowAsset;
class UPDWQuestConfig;

UCLASS()
class PDWEDITOR_API UPDWCollectQuestTargetLocations : public UActorActionUtility
{

  GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	UFlowAsset* RootFlowAsset = {};

	UPROPERTY(EditAnywhere)
	UPDWQuestConfig* QuestConfig = {};

	UFUNCTION(BlueprintCallable, Category = "Tools|CollectQuestTargetActors")
	void CollectQuestTargetActors();
	
};
