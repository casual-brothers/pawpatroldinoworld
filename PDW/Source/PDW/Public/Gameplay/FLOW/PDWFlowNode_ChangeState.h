// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "PDWFlowNode_ChangeState.generated.h"

/**
 * 
 */
UCLASS(abstract)
class PDW_API UPDWFlowNode_ChangeState : public UFlowNode
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	virtual void ContinueToNextNode();

	UFUNCTION()
	virtual void ChangeState();

public:

	void ExecuteInput(const FName& PinName) override;

	void BindEvents();

};
