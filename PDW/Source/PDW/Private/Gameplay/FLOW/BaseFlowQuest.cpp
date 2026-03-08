// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Managers/QuestSubsystem.h"
#include "PDWGameInstance.h"

void UBaseFlowQuest::SetQuestDescription(FPDWStepDescriptionData DescriptionData)
{
	CurrentStepData.DescriptionData = DescriptionData;
}

void UBaseFlowQuest::SetQuestTargets(FPDWQuestTargetData NewTargets)
{
	CurrentStepData.TargetsData = NewTargets;
}

void UBaseFlowQuest::SetCurrentQuestStepData(FPDWQuestStepData NewStepData)
{
	CurrentStepData = NewStepData;
}

FPDWQuestStepData UBaseFlowQuest::GetCurrentQuestStepData()
{
	return CurrentStepData;
}

void UBaseFlowQuest::FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance /*= true*/)
{
	UPDWGameInstance* GI = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GetFlowSubsystem() && GI && GI->GetCurrentGameContext() != EGameContext::ELoading && UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(GetWorld()))
	{
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			QuestSubsystem->EndQuest(QuestID);
		}
	}
	Super::FinishFlow(FinishPolicy, bRemoveInstance);
}