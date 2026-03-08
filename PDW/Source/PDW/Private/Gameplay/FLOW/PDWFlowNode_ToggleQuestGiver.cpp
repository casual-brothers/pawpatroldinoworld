// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_ToggleQuestGiver.h"
#include "QuestSettings.h"
#include "FlowSubsystem.h"
#include "Gameplay/Components/PDWTagComponent.h"

void UPDWFlowNode_ToggleQuestGiver::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		ToggleQuestGiver();
	}

	Super::OnPassThrough_Implementation();
}

UPDWFlowNode_ToggleQuestGiver::UPDWFlowNode_ToggleQuestGiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Quest");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_ToggleQuestGiver::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	ToggleQuestGiver();
}

#if WITH_EDITOR
FString UPDWFlowNode_ToggleQuestGiver::GetNodeDescription() const
{
	FString Description{};
	Description = FString::Printf(TEXT("%s %s as Quest Giver"), bActive? TEXT("Active") : TEXT("Remove"), *ActorTarget.ToString());
	return Description;
}
#endif

void UPDWFlowNode_ToggleQuestGiver::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	ToggleQuestGiver();
	TriggerFirstOutput(true);
}

void UPDWFlowNode_ToggleQuestGiver::ToggleQuestGiver()
{
	if(const UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		for (AActor* Actor : FlowSubsystem->GetFlowActorsByTags(ActorTarget, MatchType, AActor::StaticClass(), bSearchExactTagMatch))
		{
			if(!Actor)
				continue;

			UPDWTagComponent* TagComponent = Actor->GetComponentByClass<UPDWTagComponent>();
			if (ensureMsgf(TagComponent, TEXT("TagComponent is Missing on Actor")))
			{
				if (bActive)
				{
					TagComponent->AddTag(UQuestSettings::GetQuestGiverTag());
				}
				else
				{
					TagComponent->RemoveTag(UQuestSettings::GetQuestGiverTag());
				}
			}
		}
	}
}