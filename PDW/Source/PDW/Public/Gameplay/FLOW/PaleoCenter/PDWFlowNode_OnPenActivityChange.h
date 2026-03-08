// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_OnPenActivityChange.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Wait for Pen Activity"))
class PDW_API UPDWFlowNode_OnPenActivityChange : public UFlowNode
{
	GENERATED_BODY()
	
public:

	UPDWFlowNode_OnPenActivityChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere)
	bool WaitIsActive = true;

protected:

	void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	void OnDinoPenActivityChange(const FGameplayTag PenTag, bool IsActive);

	void OnLoad_Implementation() override;

};
