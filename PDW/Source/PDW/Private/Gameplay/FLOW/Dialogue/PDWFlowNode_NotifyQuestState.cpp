// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWFlowNode_NotifyQuestState.h"
#include "Managers/QuestSubsystem.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/FlowDeveloperSettings.h"
#include "FlowSubsystem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueEntryPointSelectorNode.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueSubGraphNode.h"

UPDWFlowNode_NotifyQuestState::UPDWFlowNode_NotifyQuestState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Quest");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_NotifyQuestState::ExecuteInput(const FName& PinName)
{
	HandleQuestNotify();

	if (bShowMessage)
	{
		Super::ExecuteInput(PinName);
	}
	else
	{
		ChangeState();
	}
}

void UPDWFlowNode_NotifyQuestState::HandleQuestNotify()
{
	if (GetFlowSubsystem())
	{
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			if(QuestMessageType == EQuestMessageType::StartQuest)
			{
				QuestSubsystem->QuestStart(Cast<UBaseFlowQuest>(GetFlowAsset()));
			}
			else if(QuestMessageType == EQuestMessageType::EndQuest)
			{
				QuestSubsystem->EndQuest(Cast<UBaseFlowQuest>(GetFlowAsset())->GetQuestID());
				if (GetFlowAsset())
				{				
					TArray<UFlowNode*> DialogueSubGraphNodes = GetFlowAsset()->GetNodesInExecutionOrder(GetFlowAsset()->GetDefaultEntryNode(), UPDWDialogueSubGraphNode::StaticClass());
					for(UFlowNode* DialogueSubGraph : DialogueSubGraphNodes)
					{
						//Remove pending quest dialogues
						if (DialogueSubGraph)
						{
							DialogueSubGraph->Finish();
						}
					}
				}
				UPDWDataFunctionLibrary::SaveGame(GetWorld());
			}
		}
	}
}

void UPDWFlowNode_NotifyQuestState::ChangeState()
{
	if (bShowMessage)
	{
		APDWGameplayGameMode* GM = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
		GM->FSMHelper->QuestMessageData.QuestMessageType = QuestMessageType;
		GM->FSMHelper->QuestMessageData.QuestName = Cast<UBaseFlowQuest>(GetFlowAsset())->GetQuestName();
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetQuestMessageTag().ToString(), "");
	}
	else
	{
		TriggerFirstOutput(true);
	}
}

void UPDWFlowNode_NotifyQuestState::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	TriggerFirstOutput(true);
}

void UPDWFlowNode_NotifyQuestState::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		HandleQuestNotify();
	}

	Super::OnPassThrough_Implementation();
}

#if WITH_EDITOR
FString UPDWFlowNode_NotifyQuestState::GetNodeDescription() const
{
	const UEnum* EnumPtr = StaticEnum<EQuestMessageType>();
    if (!EnumPtr) return FString("Invalid");

    return EnumPtr->GetNameStringByValue(static_cast<int64>(QuestMessageType));
}

#endif