// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWMinigameCompletedFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

void UPDWMinigameCompletedFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	//TODO Go To Gameplay, cutscene o other
	TriggerTransition(UFlowDeveloperSettings::GetGameplayTag().GetTagName());
}

void UPDWMinigameCompletedFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);
	if (Action == UFlowDeveloperSettings::GetGameplayTag().GetTagName())
	{
		TriggerTransition(UFlowDeveloperSettings::GetGameplayTag().GetTagName());
	}
}
