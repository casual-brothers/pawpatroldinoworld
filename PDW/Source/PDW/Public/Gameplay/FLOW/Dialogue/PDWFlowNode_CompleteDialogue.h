// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_CompleteDialogue.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Complete Dialogue"))
class PDW_API UPDWFlowNode_CompleteDialogue : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCompleteQuestDialogue = true;

	virtual void ExecuteInput(const FName& PinName) override;
	void ResetCameraAndInput();
};
