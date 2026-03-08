// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWPauseFSMState.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/FlowDeveloperSettings.h"
#include "Data/PDWGameSettings.h"
#include "PDWGameInstance.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "UI/Pages/PDWPauseMenuPage.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "Managers/QuestSubsystem.h"
#if WITH_EDITOR
#include "ToxicUtilitiesSetting.h"
#endif
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"

void UPDWPauseFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	StateOwner = InOption == "1" ? UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this) : UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	Super::OnFSMStateEnter_Implementation(InOption);
	UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, true);
	
	//TODO_UI UInputDeviceLibrary::GetAllConnectedInputDevices() Hide or show button join 2 player
	
	#if WITH_EDITOR
	if (UToxicUtilitiesSetting::Get()->bDisableTutorial)
	{
		return;
	}
	#endif

	if (UQuestSubsystem::Get(this)->GetCurrentQuestID() != QuestTutorialTag && !UPDWDataFunctionLibrary::GetFact(this, TutorialFactTag))
	{
		UPDWDataFunctionLibrary::SetFact(this, TutorialFactTag, true);
		if(UPDWPauseMenuPage* PauseMenuPage = Cast<UPDWPauseMenuPage>(PageRef))
		{
			bIsTutorialActive = true;
			for (auto Button : NavbarConfigWithPosition)
			{
				PauseMenuPage->GetPageNavbar()->DisableButton(Button.Key);
			}
			PauseMenuPage->BP_StartTutorial();
		}
	}
}

void UPDWPauseFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (bIsTutorialActive)
	{
		if (Action == UPDWGameSettings::GetSkipAction().ToString())
		{
			if(UPDWPauseMenuPage* PauseMenuPage = Cast<UPDWPauseMenuPage>(PageRef))
			{
				PauseMenuPage->BP_SkipTutorialDialogue();
			}
		}
		else if (Action == UPDWGameSettings::GetActionSuccedAnimationCompleted().ToString())
		{
			if(UPDWPauseMenuPage* PauseMenuPage = Cast<UPDWPauseMenuPage>(PageRef))
			{
				for (auto Button : NavbarConfigWithPosition)
				{
					PauseMenuPage->GetPageNavbar()->EnableButton(Button.Key);
				}
			}
			bIsTutorialActive = false;
		}
		else
		{
			return;
		}
	}
	
	if (Action == UFlowDeveloperSettings::GetGameplayTag().ToString())
	{
		BackToGameplay();
	}
	else if(Action == UPDWGameSettings::GetActionJoin2Player().ToString())
	{
		ToggleJoinMultiplayer();
	}
	else if (Action == UPDWGameSettings::GetUIActionSettings())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSettingsTag().GetTagName());
	}
	else if (Action == UPDWGameSettings::GetUIActionBack().ToString())
	{
		BackToGameplay();
	}
	else if (Action == UFlowDeveloperSettings::GetMainMenuTag().ToString())
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			DialogToHide = UNebulaFlowUIFunctionLibrary::ShowDialog(PlayerController, AskConfirmationDialogID,
				[=, this](FString inResponse)
				{
					OnGoToMainMenuDialogResponse(inResponse);
				}
			, PlayerController);
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionPrevTab().ToString())
	{
		if (UPDWPauseMenuPage* PausePage = Cast<UPDWPauseMenuPage>(PageRef))
		{
			PausePage->PrevTab();
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionNextTab().ToString())
	{
		if (UPDWPauseMenuPage* PausePage = Cast<UPDWPauseMenuPage>(PageRef))
		{
			PausePage->NextTab();
		}
	}
}

void UPDWPauseFSMState::ToggleJoinMultiplayer()
{
	UPDWGameInstance* GI = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI->GetIsMultiPlayerOn())
	{
		APDWGameplayGameMode* GM = UPDWGameplayFunctionLibrary::GetPDWGameplayGameMode(this);
		if (GM)
		{
			GM->RemovePlayer2();
		}
		BackToGameplay();
	}
	else
	{
		FName ModalIdToUse = DefaultJoinPlayerModalId;
		#if (PLATFORM_SWITCH)
			ModalIdToUse = SwitchJoinPlayerModalId;
		#endif
		DialogToHide = UNebulaFlowUIFunctionLibrary::ShowDialog(this, ModalIdToUse, [&](FString Response) {
			OnModalResponse(Response);
		});
	}
}

void UPDWPauseFSMState::OnModalResponse(FString Response)
{
	if (Response == UPDWGameSettings::GetActionJoinPlayer2Succes())
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, DialogToHide);
		BackToGameplay();
	}
	else if (Response == UPDWGameSettings::GetActionJoinPlayer2Fail())
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, DialogToHide);
	}
}


void UPDWPauseFSMState::OnGoToMainMenuDialogResponse(FString InResponse)
{
	if (InResponse == "Yes")
	{
		UPDWDataFunctionLibrary::SaveGame(this);

		if (APDWGameplayGameMode* GM = UPDWGameplayFunctionLibrary::GetPDWGameplayGameMode(this))
		{
			GM->RemovePlayer2();
		}

		UNebulaFlowAudioFunctionLibrary::ForceStopSpeaker(GetWorld());

		TriggerTransition(UFlowDeveloperSettings::GetMainMenuTag().GetTagName());
	}
	else
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, DialogToHide);
	}
}

void UPDWPauseFSMState::BackToGameplay()
{
	UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, false);
	TriggerTransition(UFlowDeveloperSettings::GetGameplayTag().GetTagName());
}

void UPDWPauseFSMState::DefineStateOwner()
{
	
}