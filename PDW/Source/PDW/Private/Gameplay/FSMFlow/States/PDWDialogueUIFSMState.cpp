// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWDialogueUIFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Data/PDWGameSettings.h"
#include "UI/Pages/PDWDialogueBasePage.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Pawns/PDWCharacter.h"

void UPDWDialogueUIFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	APlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	APlayerController* TalkingPlayer =	UPDWDialogueSubSystem::Get(this)->PlayerDoingDialogue ? UPDWDialogueSubSystem::Get(this)->PlayerDoingDialogue : P1;

	if (TalkingPlayer != P1)
	{
		//Force p1 camera on talking player camera
		P1->SetViewTargetWithBlend(
        TalkingPlayer->GetPawn(),
        0.f,
        EViewTargetBlendFunction::VTBlend_Linear
		);
	}
	UGameplayStatics::SetForceDisableSplitscreen(this,true);
}


void UPDWDialogueUIFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UFlowDeveloperSettings::GetSwitcherTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName(), "");
	}
	if (Action == UPDWGameSettings::GetStartConversationAction())
	{
		if (PageRef)
		{
			UPDWDialogueBasePage* DialoguePage = Cast<UPDWDialogueBasePage>(PageRef);
			if (DialoguePage)
			{
				DialoguePage->StartConversation();
			}
		}
	}
	if (Action == UPDWGameSettings::GetSkipAction())
	{
		if (PageRef)
		{
			UPDWDialogueBasePage* DialoguePage = Cast<UPDWDialogueBasePage>(PageRef);
			if (DialoguePage)
			{
				DialoguePage->OnSkipPressed();
			}
		}
	}
	if (Action == UPDWGameSettings::GetEndConversationAction())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName(), "");
	}
}

void UPDWDialogueUIFSMState::OnFSMStateExit_Implementation()
{
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
		APawn* Target = nullptr;

		if (P1)
		{
			if (P1->GetIsOnVehicle())
			{
				Target = P1->GetVehicleInstance();
			}
			else
			{
				Target = P1->GetPupInstance();
			}

			P1->SetViewTargetWithBlend(
				Target,
				0.f,
				EViewTargetBlendFunction::VTBlend_Linear
			);

			UGameplayStatics::SetForceDisableSplitscreen(this, false);
		}
	}

	Super::OnFSMStateExit_Implementation();
}
