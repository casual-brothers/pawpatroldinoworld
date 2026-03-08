// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWQuestMessagePage.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWQuestMessagePage : public UNebulaFlowBasePage
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void InitQuestMessage(FPDWQuestMessageData MessageData);
};
