// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "Managers/PDWEventSubsytem.h"
#include "PDWFlowNode_TriggerNebulaAction.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Trigger Nebula Action", Keywords = "TriggerAction"))
class PDW_API UPDWFlowNode_TriggerNebulaAction : public UFlowNode
{
	GENERATED_UCLASS_BODY()
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

protected:
	virtual void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	virtual void OnCallbackTrigger(const FInteractionEventSignature& inEvent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly/*, meta = (Categories = "")*/)
	FGameplayTag ActionToTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bWaitForCallback = false;
};
