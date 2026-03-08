// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWQuestMessageFSMState.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/PDWGameplayGameMode.h"
#include "UI/Pages/PDWQuestMessagePage.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "Data/FlowDeveloperSettings.h"
#include "Data/PDWGameSettings.h"

void UPDWQuestMessageFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	UPDWQuestMessagePage* QuestPage = Cast<UPDWQuestMessagePage>(PageRef);
	
	APDWGameplayGameMode* GM = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	QuestPage->InitQuestMessage(GM->FSMHelper->QuestMessageData);
}

void UPDWQuestMessageFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);
	if (Action == UPDWGameSettings::GetActionSuccedAnimationCompleted())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName(), "");
	}
}