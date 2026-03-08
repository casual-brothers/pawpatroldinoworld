// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWMinigameFSMState.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/FlowDeveloperSettings.h"
#include "PDWGameInstance.h"
#include "Managers/PDWHUDSubsystem.h"

void UPDWMinigameFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	UPDWHUDSubsystem::Get(this)->ManageHUD(EHUDControls::Show, EHUDType::Minigame);
}

void UPDWMinigameFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UFlowDeveloperSettings::GetMinigameSucceedTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetMinigameSucceedTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetMinigameCancelledTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetMinigameCancelledTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetMinigameCompletedTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetMinigameCompletedTag().GetTagName());
	}
}
