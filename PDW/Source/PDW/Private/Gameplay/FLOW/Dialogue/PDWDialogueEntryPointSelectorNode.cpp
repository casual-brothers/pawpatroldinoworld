// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWDialogueEntryPointSelectorNode.h"
#include "Managers/PDWEventSubsytem.h"
#include "Managers/QuestSubsystem.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueFlowAsset.h"


UPDWDialogueEntryPointSelectorNode::UPDWDialogueEntryPointSelectorNode()
{
#if WITH_EDITOR
	Category = TEXT("PDW|Dialogue");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
	OutputPins.Empty();
}
#if WITH_EDITOR
TArray<FFlowPin> UPDWDialogueEntryPointSelectorNode::GetContextOutputs() const
{
	TArray<FFlowPin> ContextOutputs = Super::GetContextOutputs();
	for (const FGameplayTag& Tag : EntryPoints)
	{
		ContextOutputs.AddUnique(FFlowPin(Tag.GetTagName()));
	}
	return ContextOutputs;
}

bool UPDWDialogueEntryPointSelectorNode::SupportsContextPins() const
{
	return true;
}

void UPDWDialogueEntryPointSelectorNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnReconstructionRequested.ExecuteIfBound();
}
#endif

void UPDWDialogueEntryPointSelectorNode::ExecuteInput(const FName& PinName)
{
	WaitForTalkingEvent();
}

void UPDWDialogueEntryPointSelectorNode::Cleanup()
{
	UPDWEventSubsytem::Get(this)->OnQuestTalking.RemoveDynamic(this,&UPDWDialogueEntryPointSelectorNode::OnDialogueLineTriggered);
	Super::Cleanup();
}

void UPDWDialogueEntryPointSelectorNode::WaitForTalkingEvent()
{
	UPDWEventSubsytem::Get(this)->OnQuestTalking.AddUniqueDynamic(this,&UPDWDialogueEntryPointSelectorNode::OnDialogueLineTriggered);
}

void UPDWDialogueEntryPointSelectorNode::OnDialogueLineTriggered()
{
	FName DialogueLine = UQuestSubsystem::Get(this)->GetCurrentQuestDialogueFlow()->GetEntryPoint().GetTagName();
	if (GetOutputPins().Contains(DialogueLine))
	{
		TriggerOutput(UQuestSubsystem::Get(this)->GetCurrentQuestDialogueFlow()->GetEntryPoint().GetTagName());
	}
}

void UPDWDialogueEntryPointSelectorNode::OnLoad_Implementation()
{
	WaitForTalkingEvent();
	Super::OnLoad_Implementation();
}
