// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWEggsFlow.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWEggsFlow : public UFlowAsset
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SetEggFlowConfig(FPDWEggFlowConfig EggFlowConfig);

	UFUNCTION(BlueprintCallable)
	FPDWEggFlowConfig& GetEggFlowConfig();

protected:

	UPROPERTY()
	FPDWEggFlowConfig EggFlowConfiguration;


};
