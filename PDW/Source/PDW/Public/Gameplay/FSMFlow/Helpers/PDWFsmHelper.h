// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFsmHelper.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWFsmHelper : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FPDWUnlockableContainer PendingRewards = {};

	UPROPERTY()
	FPDWQuestMessageData QuestMessageData;

	UPROPERTY()
	FPDWTriggerActionData PendingGameplayAction = {};

	UPROPERTY()
	FPDWSequenceCollection PendingSequence = {};

	UPROPERTY()
	FGameplayTag PendingDinoTagCustomization;
};
