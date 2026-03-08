// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWUIBaseElement.h"
#include "PDWProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWProgressBar : public UPDWUIBaseElement
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitProgressBar(float CurrentPercentage, float MaxPercentage);
	
	UFUNCTION(BlueprintCallable)
	void ChangeProgressPercentage(float NewPercentage);

protected:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName="InitProgressBar"))
	void BP_InitProgressBar(float CurrentPercentage, float MaxPercentage);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="ChangeProgressPercentage"))
	void BP_ChangeProgressPercentage(float NewPercentage);
};