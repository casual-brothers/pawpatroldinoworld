// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_GetEggsFlowConfig.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWFlowNode_GetEggsFlowConfig : public UFlowNode
{
	GENERATED_BODY()
		
public:

	void ExecuteInput(const FName& PinName) override;

	void InitializeInstance() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTag OutPenTag;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTag OutEggsTag;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTagContainer OutNestTag;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_GameplayTag OutNestTeleportTag;

	UFUNCTION()
	void InitOutputPins();

};
