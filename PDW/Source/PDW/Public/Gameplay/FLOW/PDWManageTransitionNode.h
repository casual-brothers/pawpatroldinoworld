// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "PDWManageTransitionNode.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Manage Transition"))
class PDW_API UPDWManageTransitionNode : public UFlowNode
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GetOptions = "PDWUIFunctionLibrary.GetTrasitionOption"))
	FName TransitionOption;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bContinueImmediately = false;	

	void OnLoad_Implementation() override;
	void OnPassThrough_Implementation() override;

	UFUNCTION()
	void ManageTransition();

	UFUNCTION()
	void OnTransitionEnd();

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

public:
	void ExecuteInput(const FName& PinName) override;
};
