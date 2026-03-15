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
#include "Managers/PDWStreamPlaySubsystem.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/NebulaFlowUIConstants.h"
#include "UI/NebulaFlowDialogDataStructures.h"
#include "UI/NebulaFlowNavbarDataStructures.h"
#include "Engine/DataTable.h"
#if WITH_EDITOR
#include "ToxicUtilitiesSetting.h"
#endif
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"

void UPDWPauseFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	StateOwner = InOption == "1" ? UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this) : UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	Super::OnFSMStateEnter_Implementation(InOption);
	UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, true);

#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (UPDWStreamPlaySubsystem* StreamPlaySubsystem = UPDWStreamPlaySubsystem::Get(this))
	{
		StreamPlaySubsystem->OnSessionChanged.RemoveAll(this);
		StreamPlaySubsystem->OnSessionChanged.AddUObject(this, &ThisClass::OnGuestSessionChanged);
	}
#endif

#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	RegisterGuestStreamPlayNavbarButton();
#endif
	
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
	else if (Action == UPDWGameSettings::GetActionGuestStreamPlay().ToString())
	{
		ToggleGuestStreamPlay();
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

void UPDWPauseFSMState::OnFSMStateExit_Implementation()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (UPDWStreamPlaySubsystem* StreamPlaySubsystem = UPDWStreamPlaySubsystem::Get(this))
	{
		StreamPlaySubsystem->OnSessionChanged.RemoveAll(this);
	}

	if (GuestWaitingDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, GuestWaitingDialog);
		GuestWaitingDialog = nullptr;
	}

	GetWorld()->GetTimerManager().ClearTimer(GuestWaitingTimeoutHandle);
#endif

	Super::OnFSMStateExit_Implementation();
}

void UPDWPauseFSMState::ToggleGuestStreamPlay()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (UPDWStreamPlaySubsystem* StreamPlaySubsystem = UPDWStreamPlaySubsystem::Get(this))
	{
		if (StreamPlaySubsystem->IsSessionActive())
		{
			StreamPlaySubsystem->StopGuestSession();
			return;
		}

		if (!StreamPlaySubsystem->IsSessionPending() && StreamPlaySubsystem->StartGuestSession())
		{
			GuestWaitingDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, GuestWaitingDialogID, [&](FString Response)
			{
				OnGuestWaitingDialogResponse(Response);
			});

			if (GuestWaitingTimeout > 0.0f)
			{
				GetWorld()->GetTimerManager().SetTimer(GuestWaitingTimeoutHandle, this, &ThisClass::OnGuestWaitingTimedOut, GuestWaitingTimeout, false);
			}
		}
	}
#endif
}

void UPDWPauseFSMState::OnGuestWaitingDialogResponse(FString Response)
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (Response == TEXT("CancelGuestStreamPlay"))
	{
		if (UPDWStreamPlaySubsystem* StreamPlaySubsystem = UPDWStreamPlaySubsystem::Get(this))
		{
			StreamPlaySubsystem->StopGuestSession();
		}

		if (GuestWaitingDialog)
		{
			UNebulaFlowUIFunctionLibrary::HideDialog(this, GuestWaitingDialog);
			GuestWaitingDialog = nullptr;
		}

		GetWorld()->GetTimerManager().ClearTimer(GuestWaitingTimeoutHandle);
		RefreshGuestNavbarButton();
	}
#endif
}

void UPDWPauseFSMState::OnGuestSessionChanged(bool bIsSessionActive)
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (bIsSessionActive)
	{
		if (GuestWaitingDialog)
		{
			UNebulaFlowUIFunctionLibrary::HideDialog(this, GuestWaitingDialog);
			GuestWaitingDialog = nullptr;
		}

		GetWorld()->GetTimerManager().ClearTimer(GuestWaitingTimeoutHandle);
		RefreshGuestNavbarButton();
		BackToGameplay();
	}
	else
	{
		RefreshGuestNavbarButton();
	}
#endif
}

void UPDWPauseFSMState::OnGuestWaitingTimedOut()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (UPDWStreamPlaySubsystem* StreamPlaySubsystem = UPDWStreamPlaySubsystem::Get(this))
	{
		if (StreamPlaySubsystem->IsSessionPending())
		{
			StreamPlaySubsystem->StopGuestSession();
		}
	}

	if (GuestWaitingDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, GuestWaitingDialog);
		GuestWaitingDialog = nullptr;
	}

	RefreshGuestNavbarButton();
#endif
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

#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
		if (UPDWStreamPlaySubsystem* StreamPlaySubsystem = UPDWStreamPlaySubsystem::Get(this))
		{
			StreamPlaySubsystem->StopGuestSession();
		}
#endif

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

void UPDWPauseFSMState::RegisterGuestStreamPlayNavbarButton()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (PageRef == nullptr || PageRef->GetPageNavbar() == nullptr)
	{
		return;
	}

	UNebulaFlowUIConstants* UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this);
	if (UIConstants == nullptr)
	{
		return;
	}

	UDataTable* NavbarButtonsTable = const_cast<UDataTable*>(UIConstants->GetNavbarButtonsTable());
	if (NavbarButtonsTable == nullptr)
	{
		return;
	}

	const FName GuestActionName = UPDWGameSettings::GetActionGuestStreamPlay();
	const bool bIsGuestActive = UPDWStreamPlaySubsystem::Get(this) && UPDWStreamPlaySubsystem::Get(this)->IsSessionActive();
	if (NavbarButtonsTable->FindRow<FNavbarButtonDataTableRow>(GuestActionName, TEXT("RegisterGuestStreamPlayNavbarButton"), false) == nullptr)
	{
		FNavbarButtonDataTableRow GuestRow;
		GuestRow.NavbarButtonClass = UPDWGameSettings::GetNavButtonClassReference();
		GuestRow.NavbarButtonData.ButtonAction = GuestActionName;
		GuestRow.NavbarButtonData.ButtonText = FText::FromString(bIsGuestActive ? TEXT("Stop Guest") : TEXT("Guest"));
		GuestRow.NavbarButtonData.bListenAction = false;
		NavbarButtonsTable->AddRow(GuestActionName, GuestRow);
	}
	else if (FNavbarButtonDataTableRow* ExistingGuestRow = NavbarButtonsTable->FindRow<FNavbarButtonDataTableRow>(GuestActionName, TEXT("RegisterGuestStreamPlayNavbarButton"), false))
	{
		ExistingGuestRow->NavbarButtonData.ButtonText = FText::FromString(bIsGuestActive ? TEXT("Stop Guest") : TEXT("Guest"));
	}

	PageRef->GetPageNavbar()->RemoveNavbarButton(GuestActionName);
	PageRef->GetPageNavbar()->AddNavbarButton(GuestActionName, ENavElementPosition::RIGHT);

	UDataTable* DialogConfigTable = const_cast<UDataTable*>(UIConstants->GetDialogConfigurationsTable());
	if (DialogConfigTable == nullptr)
	{
		return;
	}

	if (DialogConfigTable->FindRow<FDialogConfigTableRow>(GuestWaitingDialogID, TEXT("RegisterGuestStreamPlayNavbarButton"), false) == nullptr)
	{
		const FDialogConfigTableRow* TemplateRow = DialogConfigTable->FindRow<FDialogConfigTableRow>(AskConfirmationDialogID, TEXT("RegisterGuestStreamPlayNavbarButton"), false);
		if (TemplateRow != nullptr)
		{
			FDialogConfigTableRow WaitingRow = *TemplateRow;
			WaitingRow.DialogConfiguration.DialogText = FText::FromString(TEXT("Waiting for a guest to connect..."));
			WaitingRow.DialogConfiguration.bHasSecondaryText = true;
			WaitingRow.DialogConfiguration.SecondaryDialogText = FText::FromString(TEXT("You can cancel and keep playing if no one joins."));
			WaitingRow.DialogConfiguration.ButtonConfigurations.SetNum(1);
			WaitingRow.DialogConfiguration.ButtonConfigurations[0].NavButtonData.ButtonText = FText::FromString(TEXT("Cancel"));
			WaitingRow.DialogConfiguration.ButtonConfigurations[0].ButtonResponse = TEXT("CancelGuestStreamPlay");
			DialogConfigTable->AddRow(GuestWaitingDialogID, WaitingRow);
		}
	}
#endif
}

void UPDWPauseFSMState::RefreshGuestNavbarButton()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	RegisterGuestStreamPlayNavbarButton();
#endif
}
