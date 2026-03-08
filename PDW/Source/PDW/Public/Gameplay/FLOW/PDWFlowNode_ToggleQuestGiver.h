// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "PDWFlowNode_ToggleQuestGiver.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Toggle Quest Giver", Keywords = "QuestGiver"))
class PDW_API UPDWFlowNode_ToggleQuestGiver : public UFlowNode
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer ActorTarget = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGameplayContainerMatchType MatchType = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSearchExactTagMatch = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bActive = true;

	void OnLoad_Implementation() override;


	void OnPassThrough_Implementation() override;

public:
	UPDWFlowNode_ToggleQuestGiver(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	FString GetNodeDescription() const override;
#endif

	void ExecuteInput(const FName& PinName) override;

	virtual void ToggleQuestGiver();
};