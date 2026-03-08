// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_TogglePenActivity.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Toggle Pen Activity"))
class PDW_API UPDWFlowNode_TogglePenActivity : public UFlowNode
{
	GENERATED_BODY()

public:

	UPDWFlowNode_TogglePenActivity(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere)
	FGameplayTag DinoPenTag = {};

	UPROPERTY(EditAnywhere)
	bool IsActive = {};

	UPROPERTY(EditAnywhere)
	bool ResetNeedTimer = true;

protected:

	virtual void ExecuteInput(const FName& PinName) override;
	

	void OnPassThrough_Implementation() override;

};
