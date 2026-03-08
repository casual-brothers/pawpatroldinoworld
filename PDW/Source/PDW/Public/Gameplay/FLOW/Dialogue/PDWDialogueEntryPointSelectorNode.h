// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWDialogueEntryPointSelectorNode.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Entry Point Selector"))
class PDW_API UPDWDialogueEntryPointSelectorNode : public UFlowNode
{
	GENERATED_BODY()
	
	UPDWDialogueEntryPointSelectorNode();

public:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FGameplayTagContainer EntryPoints;

#if WITH_EDITOR
	TArray<FFlowPin> GetContextOutputs() const override;
	bool SupportsContextPins() const override;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void ExecuteInput(const FName& PinName) override;
	void Cleanup() override;

protected:
	
	void WaitForTalkingEvent();

	UFUNCTION()
	void OnDialogueLineTriggered();

	void OnLoad_Implementation() override;

};
