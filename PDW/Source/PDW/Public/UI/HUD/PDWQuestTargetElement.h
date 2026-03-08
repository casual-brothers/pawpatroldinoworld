// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "PDWQuestTargetElement.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWQuestTargetElement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitTargetElement"))
	void BP_InitTargetElement(FGameplayTagContainer TargetId);

	UFUNCTION(BlueprintCallable)
	void OnFoundTarget();

	UFUNCTION(BlueprintPure)
	bool IsFounded() {return bIsFound;};

protected:
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsFound = false;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnFoundTarget"))
	void BP_OnFoundTarget();
};
