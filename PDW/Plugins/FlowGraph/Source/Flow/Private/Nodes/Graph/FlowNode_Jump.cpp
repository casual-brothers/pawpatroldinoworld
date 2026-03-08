// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/Graph/FlowNode_Jump.h"
#include "FlowAsset.h"


UFlowNode_Jump::UFlowNode_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OutputPins.Empty();
}

void UFlowNode_Jump::ExecuteInput(const FName& PinName)
{
	GetFlowAsset()->TriggerJump(EventName);
	Finish();
}

