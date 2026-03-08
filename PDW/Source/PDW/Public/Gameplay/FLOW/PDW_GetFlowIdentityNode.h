// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDW_GetFlowIdentityNode.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDW_GetFlowIdentityNode : public UFlowNode
{
	GENERATED_BODY()
	
public:

	void ExecuteInput(const FName& PinName) override;


	FFlowDataPinResult_GameplayTagContainer TrySupplyDataPinAsGameplayTagContainer_Implementation(const FName& PinName) const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTagContainer OutIdentityTag;
};
