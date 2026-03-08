// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_NotifyMinigameComp.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWFlowNode_NotifyMinigameComp : public UFlowNode
{
	GENERATED_UCLASS_BODY()
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

protected:

	virtual void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame")
	FGameplayTag EventTag = FGameplayTag::EmptyTag;
};
