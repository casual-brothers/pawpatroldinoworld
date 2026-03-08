// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "GameplayTagContainer.h"
#include "Data/PDWGameplayStructures.h"
#include "BaseFlowQuest.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestUpdateSignature, UBaseFlowQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestTextUpdateSignature, FPDWStepDescriptionData, Description);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestTargetUpdateSignature, FPDWQuestTargetData, QuestTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStepStartSignature, FPDWQuestStepData, InitStepData);

class UQuestSubsystem;
class UPDWInteractionReceiverComponent;

/**
 * 
 */
UCLASS(BlueprintType)
class PDW_API UBaseFlowQuest : public UFlowAsset
{
	GENERATED_BODY()
	friend class UQuestSubsystem;

public:

	UFUNCTION(BlueprintPure)
	virtual FText GetQuestName() const { return QuestName; }

	UFUNCTION(BlueprintPure)
	virtual FGameplayTag GetQuestID() const { return QuestID; }

	UFUNCTION(BlueprintPure)
	virtual FGameplayTag GetQuestArea() const { return QuestArea; }

	UFUNCTION(BlueprintPure)
	virtual FPDWQuestStepData GetCurrentStepData() const { return CurrentStepData; }
	
	UFUNCTION(BlueprintPure)
	virtual FPDWQuestTargetData GetCurrentStepTargetsData() const { return CurrentStepData.TargetsData; }

	UFUNCTION(BlueprintPure)
	virtual FPDWStepDescriptionData GetCurrentStepDescriptionData() const { return CurrentStepData.DescriptionData; }

	//UFUNCTION(BlueprintPure)
	//virtual TArray<FText> GetQuestDescriptionHistory() const { return QuestDescriptionHistory; }


	UPROPERTY(BlueprintAssignable)
	FOnQuestTextUpdateSignature OnQuestDescriptionUpdated;

	//UPROPERTY(BlueprintAssignable)
	//FOnQuestTargetUpdateSignature OnQuestTargetUpdated;

protected:

	void SetQuestDescription(FPDWStepDescriptionData DescriptionData);

	void SetQuestTargets(FPDWQuestTargetData NewTargets);

	void SetCurrentQuestStepData(FPDWQuestStepData NewStepData);

	FPDWQuestStepData GetCurrentQuestStepData();

	void FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance = true) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag QuestID;

	UPROPERTY(EditAnywhere)
	FGameplayTag QuestArea;

	UPROPERTY(EditAnywhere)
	FText QuestName;

	UPROPERTY(SaveGame)
	FPDWQuestStepData CurrentStepData;

};
