// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/InteractionSelectorNode.h"
#include "Components/InteractionFlowComponent.h"


FName UInteractionSelectorNode::INPIN_InteractionInfoInstancedStruct;


UInteractionSelectorNode::UInteractionSelectorNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Interaction");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
	OutputPins.Empty();
	INPIN_InteractionInfoInstancedStruct = GET_MEMBER_NAME_CHECKED(UInteractionSelectorNode, InInteractionInfo);
}

void UInteractionSelectorNode::ExecuteInput(const FName& PinName)
{
	for (const auto& [Tag, Priority] : InteractionTypePriorityMap)
	{
		if (!InteractionTypes.HasTag(Tag) && Tag.IsValid())
		{
			InteractionTypes.AddTag(Tag);
		}
	}
	FInstancedStruct ResolvedInstancedStruct = ResolveInstancedStruct();
	OutInteractionInfo = ResolvedInstancedStruct;
	if (const FInteractionInfo* InteractionInfo = ResolvedInstancedStruct.GetPtr<FInteractionInfo>())
	{
		const FGameplayTagContainer FilteredInteraction = InteractionInfo->InteractionTypes.Filter(InteractionTypes);
		FGameplayTag BestInteraction = FilteredInteraction.GetByIndex(0);
		if (InteractionSelectionMode == EInteractionSelectionMode::Priority)
		{
			BestInteraction = GetHighestPriorityInteraction(FilteredInteraction);
		}
		TriggerOutput(BestInteraction.GetTagName());
	}
}

FGameplayTag UInteractionSelectorNode::GetHighestPriorityInteraction(const FGameplayTagContainer& inPossibleInteractions)
{
	int32 CurrentInteractionPriority = INT_MAX;
	FGameplayTag FoundTag = FGameplayTag::EmptyTag;

	for (const FGameplayTag& InteractionTag : inPossibleInteractions)
	{
		if (InteractionTypePriorityMap[InteractionTag] < CurrentInteractionPriority)
		{
			FoundTag = InteractionTag;
		}
	}

	return FoundTag;
}

#pragma region NODE UTILITY
FInstancedStruct UInteractionSelectorNode::ResolveInstancedStruct() const
{
	FFlowDataPinResult_InstancedStruct InstancedStructResult = TryResolveDataPinAsInstancedStruct(INPIN_InteractionInfoInstancedStruct);

	if (InstancedStructResult.Result == EFlowDataPinResolveResult::FailedMissingPin)
	{
		InstancedStructResult.Result = EFlowDataPinResolveResult::Success;
		InstancedStructResult.Value = InInteractionInfo;
	}

	check(InstancedStructResult.Result == EFlowDataPinResolveResult::Success);

	return static_cast<FInstancedStruct>(InstancedStructResult.Value);
}

#if WITH_EDITOR	
bool UInteractionSelectorNode::SupportsContextPins() const
{
	return true;
}

void UInteractionSelectorNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	InteractionTypes.Reset();
	for (const auto& [Tag, Priority] : InteractionTypePriorityMap)
	{
		if (!InteractionTypes.HasTag(Tag) && Tag.IsValid())
		{
			InteractionTypes.AddTag(Tag);
		}
	}
	OnReconstructionRequested.ExecuteIfBound();
}

TArray<FFlowPin> UInteractionSelectorNode::GetContextOutputs() const
{
	TArray<FFlowPin> ContextOutputs = Super::GetContextOutputs();
	for (const FGameplayTag& Tag : InteractionTypes)
	{
		ContextOutputs.AddUnique(FFlowPin(Tag.GetTagName()));
	}
	return ContextOutputs;
}
#endif
#pragma endregion