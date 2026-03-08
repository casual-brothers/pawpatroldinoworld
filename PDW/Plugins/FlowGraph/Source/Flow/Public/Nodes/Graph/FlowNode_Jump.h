// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Nodes/Graph/FlowNode_CustomEventBase.h"
#include "FlowNode_Jump.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Jump To"))
class FLOW_API UFlowNode_Jump : public UFlowNode_CustomEventBase
{
	GENERATED_UCLASS_BODY()
protected:

	void ExecuteInput(const FName& PinName) override;
};
