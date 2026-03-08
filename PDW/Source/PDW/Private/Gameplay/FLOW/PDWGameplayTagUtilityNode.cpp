// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWGameplayTagUtilityNode.h"

FName UPDWGameplayTagUtilityNode::INPIN_TagContainerToCheck;
FName UPDWGameplayTagUtilityNode::OUTPIN_False(TEXT("False"));
FName UPDWGameplayTagUtilityNode::OUTPIN_True(TEXT("True"));


UPDWGameplayTagUtilityNode::UPDWGameplayTagUtilityNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Utility");
	NodeDisplayStyle =FlowNodeStyle::InOut;
#endif
	OutputPins.Empty();
	OutputPins.Add(FFlowPin(OUTPIN_False));
	OutputPins.Add(FFlowPin(OUTPIN_True));
	
	INPIN_TagContainerToCheck = GET_MEMBER_NAME_CHECKED(UPDWGameplayTagUtilityNode, InContainer);
}

FGameplayTagContainer UPDWGameplayTagUtilityNode::ResolveNotifyContainer() const
{
	FFlowDataPinResult_GameplayTagContainer NotifyContainerResult = TryResolveDataPinAsGameplayTagContainer(INPIN_TagContainerToCheck);

	if (NotifyContainerResult.Result == EFlowDataPinResolveResult::FailedMissingPin)
	{
		NotifyContainerResult.Result = EFlowDataPinResolveResult::Success;
		NotifyContainerResult.Value = InContainer.Value;
	}

	check(NotifyContainerResult.Result == EFlowDataPinResolveResult::Success);

	return static_cast<FGameplayTagContainer>(NotifyContainerResult.Value);
}

void UPDWGameplayTagUtilityNode::ExecuteInput(const FName& PinName)
{
	FGameplayTagContainer ResolvedContainer = ResolveNotifyContainer();
	if (Query.Matches(ResolvedContainer))
	{
		TriggerOutput(OUTPIN_True);
	}
	else
	{
		TriggerOutput(OUTPIN_False);
	}
}
