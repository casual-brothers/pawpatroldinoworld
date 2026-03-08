// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Data/PDWGameplayStructures.h"
#include "PDWUIBaseElement.h"
#include "PDWHUDQuestLog.generated.h"

class UBaseFlowQuest;

/**
 * 
 */
UCLASS()
class PDW_API UPDWHUDQuestLog : public UPDWUIBaseElement
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void InitQuestLog();

	UFUNCTION()
	void OnQuestDescriptionUpdate(FPDWStepDescriptionData Description);

protected:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitQuestLog"))
	void BP_InitQuestLog(FPDWQuestStepData StepData);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnQuestDataUpdate"))
	void BP_OnQuestDescriptionUpdate(FPDWStepDescriptionData Description);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnQuestEnd"))
	void BP_OnQuestEnd();
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnQuestStart"))
	void BP_OnQuestStart();
	
	UFUNCTION()
	void OnStepStart(FPDWQuestStepData InitStepData);

	UFUNCTION()
	void OnQuestEnd(UBaseFlowQuest* Quest);

	UFUNCTION()
	void OnQuestStart(UBaseFlowQuest* Quest);

	void NativeDestruct() override;
};
