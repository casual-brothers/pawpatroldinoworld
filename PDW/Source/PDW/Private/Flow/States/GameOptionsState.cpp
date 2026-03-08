// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Flow/States/GameOptionsState.h"
#include "UI/Pages/GameOptionsPage.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Managers/PDWEventSubsytem.h"
#include "Data/PDWGameSettings.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

#if PLATFORM_SWITCH
#include "PDWGameInstance.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#endif

void UGameOptionsState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	GameOptionsPage = Cast<UGameOptionsPage>(PageRef);
	EventSubsystemRef = UPDWEventSubsytem::Get(this);

#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.AddUniqueDynamic(this, &ThisClass::OnClosedSwitchRemap);
	OnClosedSwitchRemap(UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0));
#endif

	if (EventSubsystemRef)
	{
		EventSubsystemRef->OnRemappingButtonClick.AddUniqueDynamic(this, &UGameOptionsState::OnRemappingButtonClick);
		//EventSubsystemRef->OnRemappingButtonNewKeyChosen.AddUniqueDynamic(this, &UGameOptionsState::OnRemappingButtonNewKeyChosen);
		EventSubsystemRef->OnOverlayCloseAnimationFinished.AddUniqueDynamic(this, &UGameOptionsState::OnOverlayClosed);
	}
}

void UGameOptionsState::OnFSMStateExit_Implementation()
{
#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.RemoveAll(this);
#endif

	if (EventSubsystemRef)
	{
		EventSubsystemRef->OnRemappingButtonClick.RemoveAll(this);
		EventSubsystemRef->OnOverlayCloseAnimationFinished.RemoveAll(this);
	}

	Super::OnFSMStateExit_Implementation();
}

void UGameOptionsState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UPDWGameSettings::GetUIActionConfirm())
	{
		if(!bInsideCategory)
		{
			bInsideCategory = true;
			GameOptionsPage->EnterCurrentCategory();
		}
		else if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (GameOptionsPage && GameOptionsPage->GetCurrentCategory() != EGameOptionsCategory::Controls)
			{
				GameOptionsPage->ConfirmCurrentGameOptions();
			}
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionBack() && !bIsRemapping)
	{
		if (bInsideCategory)
		{
			bInsideCategory = false;
			GameOptionsPage->ExitCategory();
		}
		else
		{
			UPDWDataFunctionLibrary::SaveGame(GetWorld());

			TriggerTransition(UFlowDeveloperSettings::GetBackTag().GetTagName());
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionReset() && !bIsRemapping)
	{
		if (!bInsideCategory)
		{
			return;
		}
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			ResetSettingsDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(PlayerController, AskConfirmationDialogID,
				[=, this](FString inResponse)
				{
					OnResetSettingsDialog(inResponse);
				}
			, PlayerController);
		}
	}
}

void UGameOptionsState::OnClosedSwitchRemap(bool bSingleJoyCon)
{
#if PLATFORM_SWITCH
	if(bSingleJoyCon)
	{
		bIsRemapping = false;
	}
	else if (ResetSettingsDialog)
	{
		OnResetSettingsDialog("Cancel");
	}
#endif
}

void UGameOptionsState::ResetSettings()
{
	if (GameOptionsPage)
	{
		GameOptionsPage->ResetCurrentGameOptions();
	}
}

void UGameOptionsState::OnResetSettingsDialog(FString InResponse)
{
	if (InResponse == "Yes")
	{
		ResetSettings();
	}

	if (ResetSettingsDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, ResetSettingsDialog);
	}
}

void UGameOptionsState::OnOverlayClosed()
{	
	bIsRemapping = false;
}


void UGameOptionsState::OnRemappingButtonClick(UPDWRemappingActionButton* RemappingButton)
{
	bIsRemapping = true;
}
