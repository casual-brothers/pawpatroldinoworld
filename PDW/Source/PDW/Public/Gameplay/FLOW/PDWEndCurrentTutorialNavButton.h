// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "PDWEndCurrentTutorialNavButton.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "End Tutorial Input Trigger"))
class PDW_API UPDWEndCurrentTutorialNavButton : public UFlowNode
{
	GENERATED_BODY()
	
public:
	void ExecuteInput(const FName& PinName) override;

protected:
	void OnPassThrough_Implementation() override;

};
