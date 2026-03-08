// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNodeNotifySelf.generated.h"

/**
 * Finds the Flow Component of this graph
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Notify Owner", Keywords = "event"))
class PDW_API UPDWFlowNodeNotifySelf : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Notify")
	FGameplayTagContainer NotifyTags;

	UPROPERTY(EditAnywhere, Category = "Notify")
	EFlowNetMode NetMode;

	virtual void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
	virtual EDataValidationResult ValidateNode() override;
#endif
};
