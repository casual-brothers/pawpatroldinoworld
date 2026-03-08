// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFoodFlow.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWFoodFlow : public UFlowAsset
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SetFoodFlowConfig(FPDWFoodFlowConfig FoodFlowConfig);

	UFUNCTION(BlueprintCallable)
	FPDWFoodFlowConfig& GetFoodFlowConfig();

protected:

	UPROPERTY()
	FPDWFoodFlowConfig FoodFlowConfiguration;
	
};
