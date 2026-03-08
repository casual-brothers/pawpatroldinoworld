// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWFlowNode_ConversationByTag.h"

UPDWFlowNode_ConversationByTag::UPDWFlowNode_ConversationByTag(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	InputPins.Add(FFlowPin(TEXT("TargetTag"), EFlowPinType::GameplayTagContainer));
#if WITH_EDITOR
	Category = TEXT("PDW|Dialogue");
	NodeDisplayStyle = FlowNodeStyle::Node;
#endif
	bHideConversationProperty = true;
}

void UPDWFlowNode_ConversationByTag::ExecuteInput(const FName& PinName)
{
	FGameplayTagContainer TagsToSearch = UFlowNodeBase::TryResolveDataPinAsGameplayTagContainer("TargetTag").Value;
	for (FConversationByTagDataEntry ConversationDataEntry : Conversations)
	{
		if (ConversationDataEntry.TagsContainer.HasAnyExact(TagsToSearch))
		{
			Conversation = ConversationDataEntry.Conversation;
			break;
		}
	}
	
	Super::ExecuteInput(PinName);
}