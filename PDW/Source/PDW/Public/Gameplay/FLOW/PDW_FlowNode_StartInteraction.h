// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDW_FlowNode_StartInteraction.generated.h"

class UFlowComponent;
class UPDWInteractionReceiverComponent;

UCLASS(NotBlueprintable, meta = (DisplayName = "Start Interaction"))
class PDW_API UPDW_FlowNode_StartInteraction : public UFlowNode
{
	GENERATED_BODY()
	
public:
	UPDW_FlowNode_StartInteraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FGameplayTagContainer InteractableTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	EGameplayContainerMatchType MatchType = {};


	UPROPERTY(EditAnywhere, Category = "Interaction")
	FGameplayTag InteractionType;

	virtual void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	virtual void OnComponentRegistered(UFlowComponent* Component);

	UFUNCTION()
	void ProcessInteractionAndContinue(UPDWInteractionReceiverComponent* InteractionComp);

};
