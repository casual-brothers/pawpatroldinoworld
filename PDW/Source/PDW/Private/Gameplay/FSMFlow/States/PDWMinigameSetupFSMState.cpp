// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWMinigameSetupFSMState.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/FlowDeveloperSettings.h"
#include "PDWGameInstance.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWMinigameSetupFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	UPDWHUDSubsystem::Get(this)->ManageHUD(EHUDControls::Create, EHUDType::Minigame);
	UPDWHUDSubsystem::Get(this)->ManageHUD(EHUDControls::Hide, EHUDType::Minigame);
	FInteractionEventSignature EventSignature = FInteractionEventSignature();
	EventSignature.EventTag = UFlowDeveloperSettings::GetMinigameSetupTag();
	UPDWEventSubsytem::Get(this)->TriggerInteractionEvent(UFlowDeveloperSettings::GetMinigameSetupTag(), EventSignature);
}

void UPDWMinigameSetupFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UFlowDeveloperSettings::GetMinigameTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetMinigameTag().GetTagName());
	}
}
