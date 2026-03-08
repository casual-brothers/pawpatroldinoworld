// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWTalkAndWaitNode.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueFlowAsset.h"
#include "Managers/QuestSubsystem.h"
#include "Managers/PDWEventSubsytem.h"


UPDWTalkAndWaitNode::UPDWTalkAndWaitNode()
{
#if WITH_EDITOR
	Category = TEXT("PDW|Dialogue");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
}

void UPDWTalkAndWaitNode::ExecuteInput(const FName& PinName)
{
	SetDialogueEntryPoint();
	Super::ExecuteInput(PinName);
}

void UPDWTalkAndWaitNode::OnQuestTalking()
{
	UPDWEventSubsytem::Get(this)->OnQuestTalking.RemoveDynamic(this, &ThisClass::OnQuestTalking);
	bIsTriggered = true;
}

#if WITH_EDITOR
FString UPDWTalkAndWaitNode::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s"), DialogueID.IsValid() ? *DialogueID.ToString() : TEXT("Missing Dialogue ID"));
}
#endif


void UPDWTalkAndWaitNode::ContinueToNextNode()
{
	if (bIsTriggered)
	{
		Super::ContinueToNextNode();
	}
}

void UPDWTalkAndWaitNode::SetDialogueEntryPoint()
{
	UPDWDialogueFlowAsset* DialogueAsset = UQuestSubsystem::Get(this)->GetCurrentQuestDialogueFlow();
	if (DialogueAsset)
	{
		DialogueAsset->SetEntryPoint(DialogueID);
		UPDWEventSubsytem::Get(this)->OnQuestTalking.AddUniqueDynamic(this, &ThisClass::OnQuestTalking);
	}
}

void UPDWTalkAndWaitNode::OnLoad_Implementation()
{
	BindEvents();
	SetDialogueEntryPoint();
	Super::OnLoad_Implementation();
}
