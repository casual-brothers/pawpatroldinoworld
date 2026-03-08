// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWTagUtilityNode.h"

FName UPDWTagUtilityNode::INPIN_TagToCheck;
FName UPDWTagUtilityNode::OUTPIN_False(TEXT("False"));
FName UPDWTagUtilityNode::OUTPIN_True(TEXT("True"));

UPDWTagUtilityNode::UPDWTagUtilityNode(const FObjectInitializer& ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Utility");
	NodeDisplayStyle =FlowNodeStyle::InOut;
#endif
	OutputPins.Empty();
	OutputPins.Add(FFlowPin(OUTPIN_False));
	OutputPins.Add(FFlowPin(OUTPIN_True));
	
	INPIN_TagToCheck = GET_MEMBER_NAME_CHECKED(UPDWTagUtilityNode, InTag);
}

FGameplayTag UPDWTagUtilityNode::ResolveGameplayTag() const
{
	FFlowDataPinResult_GameplayTag NotifyContainerResult = TryResolveDataPinAsGameplayTag(INPIN_TagToCheck);

	if (NotifyContainerResult.Result == EFlowDataPinResolveResult::FailedMissingPin)
	{
		NotifyContainerResult.Result = EFlowDataPinResolveResult::Success;
		NotifyContainerResult.Value = InTag.Value;
	}

	check(NotifyContainerResult.Result == EFlowDataPinResolveResult::Success);

	return static_cast<FGameplayTag>(NotifyContainerResult.Value);
}

void UPDWTagUtilityNode::ExecuteInput(const FName& PinName)
{
	FGameplayTag ResolvedContainer = ResolveGameplayTag();
	if (ResolvedContainer.MatchesAny(ContainerToCheck))
	{
		TriggerOutput(OUTPIN_True);
	}
	else
	{
		TriggerOutput(OUTPIN_False);
	}
}
