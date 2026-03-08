// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_GetFoodFlowConfig.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWFlowNode_GetFoodFlowConfig : public UFlowNode
{
	GENERATED_BODY()
		
public:

	void ExecuteInput(const FName& PinName) override;

	void InitializeInstance() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTagContainer OutFoodPlantTag;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTagContainer OutFoodStationTag;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTag OutFoodTypeTag;

	UFUNCTION()
	void InitOutputPins();
	
};
