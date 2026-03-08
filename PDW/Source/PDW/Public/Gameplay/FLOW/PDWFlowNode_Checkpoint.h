// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_Checkpoint.generated.h"

/**
 * Save the state of the game to the save file
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Checkpoint", Keywords = "autosave, save"))
class PDW_API UPDWFlowNode_Checkpoint : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void ExecuteInput(const FName& PinName) override;

	void OnLoad_Implementation() override;

};
