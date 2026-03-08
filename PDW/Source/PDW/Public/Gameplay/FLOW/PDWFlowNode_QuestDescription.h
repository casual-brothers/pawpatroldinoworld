// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFlowNode_QuestDescription.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Update Quest Description"))
class PDW_API UPDWFlowNode_QuestDescription : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(EditAnywhere)
	FPDWStepDescriptionData QuestDescription;

	UFUNCTION()
	void UpdateDescription();

	void ExecuteInput(const FName& PinName) override;

	void OnPassThrough_Implementation() override;

};
