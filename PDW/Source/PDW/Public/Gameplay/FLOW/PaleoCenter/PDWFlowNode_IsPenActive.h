// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Nodes/Route/FlowNode_Branch.h"
#include "PDWFlowNode_IsPenActive.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Is Pen Active"))
class PDW_API UPDWFlowNode_IsPenActive : public UFlowNode
{
	GENERATED_BODY()

public:

	UPDWFlowNode_IsPenActive(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ExecuteInput(const FName& PinName) override;

	static const FName INPIN_Evaluate;
	static const FName OUTPIN_True;
	static const FName OUTPIN_False;
};
