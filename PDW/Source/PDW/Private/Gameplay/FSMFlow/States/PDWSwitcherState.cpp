// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWSwitcherState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Managers/PDWEventSubsytem.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Data/PDWGameplayStructures.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

void UPDWSwitcherState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UPDWGameplayFunctionLibrary::StopForceFeedbackBothPlayers(this);
	DoingTransition = false;
	PendingAction = {};

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnSwitcherStateWaitForActionEvent();
	}
	
	if (!DoingTransition)
	{
		APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);

		if (GM && GM->FSMHelper && PendingAction.Action != "")
		{
			GM->FSMHelper->PendingGameplayAction = PendingAction;
		}

		TriggerTransition(UFlowDeveloperSettings::GetGameplayTag().GetTagName());
	}
}

void UPDWSwitcherState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UFlowDeveloperSettings::GetCutsceneTag().ToString())
	{
		DoingTransition = true;
		TriggerTransitionWithOption(UFlowDeveloperSettings::GetCutsceneTag().GetTagName(), Parameter);
	}
	else if (Action == UFlowDeveloperSettings::GetDialogueTag().ToString())
	{
		DoingTransition = true;
		TriggerTransition(UFlowDeveloperSettings::GetDialogueTag().GetTagName());
	}
	else if (Action == UFlowDeveloperSettings::GetMinigameTag().ToString())
	{
		DoingTransition = true;
		TriggerTransition(UFlowDeveloperSettings::GetMinigameTag().GetTagName());
	}
	else if (Action == UFlowDeveloperSettings::GetMinigameSetupTag().ToString())
	{
		DoingTransition = true;
		TriggerTransition(UFlowDeveloperSettings::GetMinigameSetupTag().GetTagName());
	}
	else if (Action == UFlowDeveloperSettings::GetInGameCreditsTag().ToString())
	{
		DoingTransition = true;
		TriggerTransition(UFlowDeveloperSettings::GetInGameCreditsTag().GetTagName());
	}
	else if (Action != UFlowDeveloperSettings::GetGameplayTag().ToString())
	{	
		PendingAction.Action = Action;
		PendingAction.Parameter = Parameter;
		PendingAction.ControllerSender = ControllerSender;
	}
}

void UPDWSwitcherState::OnFSMStateExit_Implementation()
{
	if (APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(GetWorld()))
	{
		GM->SwitcherWaitForAction = false;
	}

	Super::OnFSMStateExit_Implementation();
}
