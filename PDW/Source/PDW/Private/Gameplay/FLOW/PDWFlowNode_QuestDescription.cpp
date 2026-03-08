// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_QuestDescription.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Managers/QuestSubsystem.h"

UPDWFlowNode_QuestDescription::UPDWFlowNode_QuestDescription(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_QuestDescription::UpdateDescription()
{
	UBaseFlowQuest* FlowQuest = Cast<UBaseFlowQuest>(GetFlowAsset());
	if (FlowQuest)
	{
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			QuestSubsystem->SetCurrentQuestDescription(QuestDescription);
		}
	}
}

void UPDWFlowNode_QuestDescription::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	UpdateDescription();
	TriggerFirstOutput(true);
}

void UPDWFlowNode_QuestDescription::OnPassThrough_Implementation()
{
	if(CheatMode)
	{
		UpdateDescription();
	}

	Super::OnPassThrough_Implementation();
}

