// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_UpdateQuestStep.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Managers/QuestSubsystem.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Components/PDWBreadCrumbsComponent.h"
#include "FlowSubsystem.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

UPDWFlowNode_UpdateQuestStep::UPDWFlowNode_UpdateQuestStep(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_UpdateQuestStep::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	StartStep();
	if (ShouldSave)
	{
		UPDWDataFunctionLibrary::SaveGame(GetWorld());
	}
	TriggerFirstOutput(true);
}

void UPDWFlowNode_UpdateQuestStep::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	StartStep();
	TriggerFirstOutput(true);
}

void UPDWFlowNode_UpdateQuestStep::StartStep()
{
	UBaseFlowQuest* FlowQuest = Cast<UBaseFlowQuest>(GetFlowAsset());
	if (FlowQuest)
	{
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			QuestSubsystem->StartStep(StepData);
		}
	}
}

void UPDWFlowNode_UpdateQuestStep::OnPassThrough_Implementation()
{
	if (CheatMode)
	{	
		StartStep();
	}

	Super::OnPassThrough_Implementation();
}

#if WITH_EDITOR
FString UPDWFlowNode_UpdateQuestStep::GetNodeDescription() const
{
	FString DescriptionString {};
	FString Tags = {};
	for (const FGameplayTag Tag : StepData.TargetsData.IdentityTag)
	{
		Tags.Append(GetIdentityTagDescription(Tag));
		Tags.Append(", ");
	}
	DescriptionString.Append(FString::Printf(TEXT("To %s"), *Tags));
	return DescriptionString;
}

#endif