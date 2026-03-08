// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/Graph/FlowNode_SubGraph.h"
#include "PDWDialogueSubGraphNode.generated.h"

class UPDWDialogueFlowAsset;

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Start Dialogue Graph"))
class PDW_API UPDWDialogueSubGraphNode : public UFlowNode_SubGraph
{
	GENERATED_BODY()
	
	UPDWDialogueSubGraphNode();

public:

	FFlowDataPinInputProperty_Object OutQuestAsset;


	void InitializeInstance() override;

protected:

	void ExecuteInput(const FName& PinName) override;

	void InitDialogueSystem();

};
