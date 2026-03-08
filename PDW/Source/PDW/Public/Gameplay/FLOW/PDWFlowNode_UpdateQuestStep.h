// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFlowNode_UpdateQuestStep.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Quest Step"))
class PDW_API UPDWFlowNode_UpdateQuestStep : public UFlowNode
{
	GENERATED_UCLASS_BODY()
public:
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

	UPROPERTY(EditAnywhere)
	FPDWQuestStepData StepData;

	UPROPERTY(EditAnywhere)
	bool ShouldSave = true;

protected:

	UFUNCTION()
	void StartStep();

	virtual void ExecuteInput(const FName& PinName) override;

	void OnPassThrough_Implementation() override;


	void OnLoad_Implementation() override;

};
