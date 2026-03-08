// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Data/NebulaInteractionSystemStructs.h"

#include "InteractionSelectorNode.generated.h"

UENUM(BlueprintType)
enum class EInteractionSelectionMode : uint8
{
	Priority			=		0,
	FirstMatch			=		1,
};
//prova a mettere l'anteprima dell'asset sulle instanced
UCLASS(NotBlueprintable, meta = (DisplayName = "InteractionSelector", Keywords = "InteractionSelector"))
class NEBULAINTERACTIONSYSTEM_API UInteractionSelectorNode : public UFlowNode
{
	GENERATED_BODY()

	UInteractionSelectorNode(const FObjectInitializer& ObjectInitializer);
public:

	UPROPERTY(EditAnywhere, Category = "Flow", meta = (DefaultForInputFlowPin, FlowPinType = InstancedStruct))
	FInstancedStruct InInteractionInfo;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FFlowDataPinOutputProperty_InstancedStruct OutInteractionInfo;

	FGameplayTagContainer InteractionTypes;

	UPROPERTY(EditAnywhere, Category = "Configuration",meta = (ForceInlineRow))
	TMap<FGameplayTag, int32> InteractionTypePriorityMap;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	EInteractionSelectionMode InteractionSelectionMode = EInteractionSelectionMode::Priority;
	#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	bool SupportsContextPins() const override;
	TArray<FFlowPin> GetContextOutputs() const override;
	#endif
	void ExecuteInput(const FName& PinName) override;

protected:

	static FName INPIN_InteractionInfoInstancedStruct;

private:

	FInstancedStruct  ResolveInstancedStruct() const;
	FGameplayTag GetHighestPriorityInteraction(const FGameplayTagContainer& inPossibleInteractions);
};
