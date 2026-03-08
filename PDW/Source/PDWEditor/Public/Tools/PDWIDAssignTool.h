// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorActionUtility.h"
#include "GameplayTagContainer.h"
#include "PDWIDAssignTool.generated.h"

UCLASS()
class PDWEDITOR_API UPDWIDAssignTool : public UActorActionUtility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void AssignTreatsID(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void AssignInteractableID(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void AssignQuestTargetID(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void CheckTreatsDuplicates(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void CheckInteractableDuplicates(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void CheckQuestTargetDuplicates(UObject* WorldContextObject);
		
	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void ResetTreatsIDs(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void ResetInteractableIDs(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Tools|IDAssignTool")
	void ResetQuestTargetIDs(UObject* WorldContextObject);

	UFUNCTION()
	int32 FindFirstAvailableId(TArray<int32>& AlreadyAssignedIDs);
};
