// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "GameplayTagContainer.h"
#include "WaitForInteractionNode.generated.h"

class UInteractionFlowComponent;
/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "WaitForInteraction", Keywords = "WaitInteraction"))
class NEBULAINTERACTIONSYSTEM_API UWaitForInteractionNode : public UFlowNode
{
	GENERATED_BODY()
	
	UWaitForInteractionNode(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FGameplayTagContainer States;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_InstancedStruct OutInteractionInfo;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	bool SupportsContextPins() const override;
	TArray<FFlowPin> GetContextOutputs() const override;
#endif
	void ExecuteInput(const FName& PinName) override;
	void WaitForEvent();
	UFUNCTION()
	void OnEventReceived(UInteractionFlowComponent* inComponent, const FInstancedStruct& inInteractionInfo);
	void Cleanup() override;

};
