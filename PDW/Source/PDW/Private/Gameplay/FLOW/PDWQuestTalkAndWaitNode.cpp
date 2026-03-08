// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWQuestTalkAndWaitNode.h"
#include "QuestSettings.h"
#include "Gameplay/Components/PDWTagComponent.h"
#include "FlowSubsystem.h"


void UPDWQuestTalkAndWaitNode::ExecuteInput(const FName& PinName)
{
	ToggleQuestGiver(true);
	Super::ExecuteInput(PinName);
}

void UPDWQuestTalkAndWaitNode::OnLoad_Implementation()
{
	ToggleQuestGiver(true);
	Super::OnLoad_Implementation();
}

void UPDWQuestTalkAndWaitNode::ToggleQuestGiver(bool IsActive)
{
	UFlowSubsystem* FlowSubsystem = GetFlowSubsystem();
	if (!FlowSubsystem) return;

	bool Found = false;
	CurrentIsActive = IsActive;

	for (AActor* Actor : FlowSubsystem->GetFlowActorsByTags(ActorTarget, MatchType, AActor::StaticClass(), bSearchExactTagMatch))
	{
		if (!Actor)
			continue;

		Found = true;
		HandleQuestGiverTag(Actor);
	}

	if (!Found)
	{
		FlowSubsystem->OnComponentRegistered.AddUniqueDynamic(this, &UPDWQuestTalkAndWaitNode::OnComponentRegistered);
	}	
}

void UPDWQuestTalkAndWaitNode::TriggerFirstOutput(const bool bFinish)
{
	ToggleQuestGiver(false);
	Super::TriggerFirstOutput(bFinish);
}

void UPDWQuestTalkAndWaitNode::OnComponentRegistered(UFlowComponent* Component)
{
	if(!Component || !Component->GetOwner()) return;

	UFlowSubsystem* FlowSubsystem = GetFlowSubsystem();
	if(!FlowSubsystem) return;

	switch (MatchType)
	{
	case EGameplayContainerMatchType::Any:
		if (!Component->IdentityTags.HasAnyExact(ActorTarget)) return;
		break;
	case EGameplayContainerMatchType::All:
		if (!Component->IdentityTags.HasAllExact(ActorTarget)) return;
		break;
	default:
		break;
	}

	HandleQuestGiverTag(Component->GetOwner());
}

void UPDWQuestTalkAndWaitNode::HandleQuestGiverTag(AActor* TagOwner)
{
	UPDWTagComponent* TagComponent = TagOwner->GetComponentByClass<UPDWTagComponent>();

	if (ensureMsgf(TagComponent, TEXT("TagComponent is Missing on Actor")))
	{
		if (CurrentIsActive)
		{
			TagComponent->AddTag(UQuestSettings::GetQuestGiverTag());
		}
		else
		{
			TagComponent->RemoveTag(UQuestSettings::GetQuestGiverTag());
		}
	}
}

void UPDWQuestTalkAndWaitNode::TriggerOutput(FName PinName, const bool bFinish /*= false*/, const EFlowPinActivationType ActivationType /*= EFlowPinActivationType::Default*/)
{
	UFlowSubsystem* FlowSubsystem = GetFlowSubsystem();
	if (FlowSubsystem && FlowSubsystem->OnComponentRegistered.IsBound())
	{
		FlowSubsystem->OnComponentRegistered.RemoveDynamic(this, &UPDWQuestTalkAndWaitNode::OnComponentRegistered);
	}

	Super::TriggerOutput(PinName, bFinish, ActivationType);
}

void UPDWQuestTalkAndWaitNode::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		ToggleQuestGiver(false);
	}

	Super::OnPassThrough_Implementation();
}
