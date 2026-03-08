// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_SetFact.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Set Fact", Keywords = "facts"))
class PDW_API UPDWFlowNode_SetFact : public UFlowNode
{
	GENERATED_UCLASS_BODY()
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

protected:
	virtual void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Facts"))
	FGameplayTag Fact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFactValue;
};
