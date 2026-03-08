// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/Graph/FlowNode_SubGraph.h"
#include "PDWFlowNode_Quest.generated.h"

class UBaseFlowQuest;

UCLASS(NotBlueprintable, meta = (DisplayName = "Start Quest Graph"))
class PDW_API UPDWFlowNode_Quest : public UFlowNode_SubGraph
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION()
	bool Cheat_CreateQuest();

	UFUNCTION()
	UBaseFlowQuest* GetBaseFlowQuest() {return FlowQuest; };

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
protected:

	UFUNCTION()
	void StartQuest();

	void ExecuteInput(const FName& PinName) override;

	virtual void OnLoad_Implementation() override;

	UPROPERTY()
	UBaseFlowQuest* FlowQuest {};

};
