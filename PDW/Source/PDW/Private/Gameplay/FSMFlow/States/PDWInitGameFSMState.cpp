// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/FSMFlow/States/PDWInitGameFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Managers/PDWEventSubsytem.h"
#include "Framework/Application/NavigationConfig.h"

void UPDWInitGameFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	FNavigationConfig& NavigationConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig.bTabNavigation = false;
	NavigationConfig.KeyEventRules.Emplace(EKeys::W, EUINavigation::Up);
	NavigationConfig.KeyEventRules.Emplace(EKeys::S, EUINavigation::Down);
    NavigationConfig.KeyEventRules.Emplace(EKeys::A, EUINavigation::Left);
    NavigationConfig.KeyEventRules.Emplace(EKeys::D, EUINavigation::Right);

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnInitGameEnter.Broadcast();
	}
}

void UPDWInitGameFSMState::OnFSMStateExit_Implementation()
{
	Super::OnFSMStateExit_Implementation();
}

void UPDWInitGameFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action,Parameter,ControllerSender);

	if (Action == UFlowDeveloperSettings::GetSwitcherTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
	}
}