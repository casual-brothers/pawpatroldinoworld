// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_FakeDinoNeed.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Fake dino need"))
class PDW_API UPDWFlowNode_FakeDinoNeed : public UFlowNode
{
	GENERATED_BODY()
	
public:

	UPDWFlowNode_FakeDinoNeed(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere)
	FGameplayTag DinoPenTag = {};

	UPROPERTY(EditAnywhere)
	FGameplayTag FakeNeed = {};

	void ExecuteInput(const FName& PinName) override;

};
