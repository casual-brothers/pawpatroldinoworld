// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWSoundtrackNode.h"
#include "Managers/PDWAudioManager.h"

FName UPDWSoundtrackNode::INPIN_NotifyContainer;

UPDWSoundtrackNode::UPDWSoundtrackNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Condition;
	Category = TEXT("Sound");
#endif
	OutputPins = { FFlowPin(TEXT("Success"))};

	INPIN_NotifyContainer = GET_MEMBER_NAME_CHECKED(UPDWSoundtrackNode, NotifyTags);
}

void UPDWSoundtrackNode::ExecuteInput(const FName& PinName)
{
	PlaySoundtrack();
	TriggerFirstOutput(true);
}

void UPDWSoundtrackNode::PlaySoundtrack()
{
	ResolvedNotifyContainer = ResolveNotifyContainer();
	if (ResolvedNotifyContainer.Num())
	{
		UPDWAudioManager::Get(this)->PlayMiniGameSoundtrack(ResolvedNotifyContainer.First());
	}
}

FGameplayTagContainer UPDWSoundtrackNode::ResolveNotifyContainer() const
{
	FFlowDataPinResult_GameplayTagContainer NotifyContainerResult = TryResolveDataPinAsGameplayTagContainer(INPIN_NotifyContainer);

	if (NotifyContainerResult.Result == EFlowDataPinResolveResult::FailedMissingPin)
	{
		// Handle lookup of a UFlowNode_Timer that predated DataPins
		NotifyContainerResult.Result = EFlowDataPinResolveResult::Success;
		NotifyContainerResult.Value = NotifyTags;
	}

	check(NotifyContainerResult.Result == EFlowDataPinResolveResult::Success);

	return static_cast<FGameplayTagContainer>(NotifyContainerResult.Value);
}