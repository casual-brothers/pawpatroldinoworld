// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited


#include "Flow/States/PDWMainMenuState.h"
#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/Pages/PDWMainMenuPage.h"
#include "Data/FlowDeveloperSettings.h"
#include "Data/PDWGameSettings.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"


void UPDWMainMenuState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	
	if (UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this))
	{
		if (PersistentUser->HasSaveGames())
		{
			const int32 currentSlotID = PersistentUser->GetCurrentUseSlotIndex();
			LoadFromSlotID(currentSlotID);
		}
	}
	if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
	{
		EventSubsystem->OnSlotOnFocus.AddUniqueDynamic(this, &UPDWMainMenuState::OnSlotFocused);
		EventSubsystem->OnPageReady.AddUniqueDynamic(this, &UPDWMainMenuState::OnPageReady);
	}
}


void UPDWMainMenuState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);
	if(!bPageReady)
	{
		return;
	}
	if (Action == UPDWGameSettings::GetUIActionSelectSlot())
	{
		// sent from button page click or navbar confirm
		int32 slotID = FCString::Atoi(*Parameter);
		if (bIsNewGame)
		{
			// Start new game calls proceed from inside because it has a callback for overriding saves
			StartNewGame(slotID);
		}
		else
		{
			if (LoadFromSlotID(slotID))
			{
				LoadGame();
			}
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionContinue())
	{
		Continue();

	}
	else if (Action == UPDWGameSettings::GetUIActionBack())
	{	
		if (bIsSelectingSlots)
		{
			bIsSelectingSlots = false;
			if (UPDWMainMenuPage* MainMenuPage = Cast<UPDWMainMenuPage>(PageRef))
			{
				MainMenuPage->SwitchToMenuButtons(!bIsNewGame);

				MainMenuPage->GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionBack());
				MainMenuPage->GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionDelete());
			}
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionLoadGame())
	{
		bIsNewGame = false;
		bIsSelectingSlots = true;
		if (UPDWMainMenuPage* MainMenuPage = Cast<UPDWMainMenuPage>(PageRef))
		{
			MainMenuPage->SwitchToSlotButtons();
			MainMenuPage->GetPageNavbar()->AddNavbarButton(UPDWGameSettings::GetUIActionBack(), ENavElementPosition::LEFT);
			MainMenuPage->GetPageNavbar()->AddNavbarButton(UPDWGameSettings::GetUIActionDelete(), ENavElementPosition::RIGHT);
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionNewGame())
	{
		bIsNewGame = true;
		bIsSelectingSlots = true;
		UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
		if (bIsNewGame && !PersistentUser->HasSaveGames())
		{
			StartNewGame(0);
		}
		else
		{
			if (UPDWMainMenuPage* MainMenuPage = Cast<UPDWMainMenuPage>(PageRef))
			{
				MainMenuPage->SwitchToSlotButtons(bIsNewGame);
				MainMenuPage->GetPageNavbar()->AddNavbarButton(UPDWGameSettings::GetUIActionBack(), ENavElementPosition::LEFT);
			}
		}

	}		
	else if (Action == UPDWGameSettings::GetUIActionDelete())
	{
		DeleteGame();
	}	
	else if (Action == UPDWGameSettings::GetUIActionSettings())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSettingsTag().GetTagName());
	}
	else if (Action == UPDWGameSettings::GetUIActionCredits())
	{
		TriggerTransition(UFlowDeveloperSettings::GetCreditsTag().GetTagName());
	}
	else if (Action == UPDWGameSettings::GetUIActionQuit())
	{
		QuitGame();
	}
}

void UPDWMainMenuState::Continue()
{
	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
	bool bSuccess = false;
	FGameProgressionSlot& SaveSlot = PersistentUser->GetGameProgressionSlotByIndex(LastFocusedSlotID, bSuccess);
	if (!bSuccess)
	{
		TArray<FGameProgressionSlot>& Slots = PersistentUser->GetGameProgressionSlots();
		for (int32 i = 0; i < Slots.Num(); i++)
		{
			SaveSlot = PersistentUser->GetGameProgressionSlotByIndex(i, bSuccess);
			if(bSuccess)
			{
				if(!SaveSlot.SlotMetaData.IsNewGame)
				{
					PersistentUser->SetCurrentUseSlotIndex(i);					
					Proceed();
				}
			}
		}
	}
	else
	{							
		Proceed();
	}
}

void UPDWMainMenuState::DeleteGame()
{
	if (bIsSelectingSlots)
	{
		UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
		bool bSuccess = false;
		FGameProgressionSlot& SaveSlot = PersistentUser->GetGameProgressionSlotByIndex(LastFocusedSlotID, bSuccess);
		if (bSuccess)
		{
			DeleteSlotDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, DeleteSlotDialogID,
				[=, this](FString InResponse)
				{
					if (InResponse == "Yes")
					{
						bool bSuccess;
						FGameProgressionSlot& SaveSlot = PersistentUser->GetGameProgressionSlotByIndex(LastFocusedSlotID, bSuccess);
						SaveSlot.SlotMetaData.IsNewGame = true;
						SaveSlot = FGameProgressionSlot();
						if (UPDWMainMenuPage* MainMenuPage = Cast<UPDWMainMenuPage>(PageRef))
						{
							MainMenuPage->Init(0);

							if (PersistentUser->HasSaveGames())
							{
								MainMenuPage->SwitchToSlotButtons(bIsNewGame);
							}
							else
							{
								bIsSelectingSlots = false;
								MainMenuPage->SwitchToMenuButtons(bIsNewGame);

								MainMenuPage->GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionBack());
								MainMenuPage->GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionDelete());
							}
						}
						UPDWDataFunctionLibrary::SaveGame(this);
					}
					UNebulaFlowUIFunctionLibrary::HideDialog(this, DeleteSlotDialog);
				}
			, StateOwner);

		}
	}
}

bool UPDWMainMenuState::LoadFromSlotID(int32 slotID)
{
	LastSelectedSlotID = slotID;

	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
	if (!ensureMsgf(PersistentUser, TEXT("Unable to retrive PersistentUser!")))
	{
		return false;
	}

	// in this case the user is selecting load game with an empty slot so we just skip
	if (PersistentUser->GetGameProgressionSlots()[slotID].SlotMetaData.SlotIndex == -1)
	{
		return false;
	}

	PersistentUser->SetCurrentUseSlotIndex(slotID);
	return true;
}

void UPDWMainMenuState::OnSlotFocused(int32 slotID)
{
	LastFocusedSlotID = slotID;
}

void UPDWMainMenuState::OnPageReady()
{
	bPageReady = true;
}

void UPDWMainMenuState::StartNewGame(int32 slotID)
{
	LastSelectedSlotID = slotID;
	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
	if (!ensureMsgf(PersistentUser, TEXT("Unable to retrive PersistentUser!")))
	{
		return;
	}
	// reserve the slot before going ahead		
	if (PersistentUser->GetGameProgressionSlots()[slotID].SlotMetaData.SlotIndex == -1)
	{
		PersistentUser->SetCurrentUseSlotIndex(slotID);
		PersistentUser->GetGameProgressionSlots()[slotID].SlotMetaData.SlotIndex = slotID;
		PersistentUser->GetGameProgressionSlots()[slotID].PlayerData.CurrentQuestDescription = ContinueLabel;
		Proceed();
		return;
	}

	// This slot already has something
	OverwriteSlotDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, OverwriteSlotDialogueID,
		[=, this](FString InResponse)
		{
			if (InResponse == "Yes")
			{
				PersistentUser->RemoveProgressionSlotByIndex(slotID, this);
				PersistentUser->SetCurrentUseSlotIndex(slotID);
				PersistentUser->GetGameProgressionSlots()[slotID].SlotMetaData.SlotIndex = slotID;
				PersistentUser->GetGameProgressionSlots()[slotID].PlayerData.CurrentQuestDescription = ContinueLabel;

				Proceed();
			}
			else if (InResponse == "No")
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(OverwriteSlotDialog, OverwriteSlotDialog);
			}
		}
	, StateOwner);

}

void UPDWMainMenuState::QuitGame()
{
	// This slot already has something
	QuitGameDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, ExitGameDialogID,
		[=, this](FString InResponse)
		{
			if (InResponse == "Yes")
			{
#if !WITH_EDITOR
				FGenericPlatformMisc::RequestExit(false);
#endif
			}
			else if (InResponse == "No")
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(this, QuitGameDialog);
			}
		}
	, StateOwner);

}


void UPDWMainMenuState::LoadGame()
{
	// This slot already has something
	LoadGameDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, LoadGameDialogID,
		[=, this](FString InResponse)
		{
			if (InResponse == "Yes")
			{				
				Proceed();
			}
			else if (InResponse == "No")
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(this, LoadGameDialog);
			}
		}
	, StateOwner);

}



void UPDWMainMenuState::Proceed()
{

	// #TODO_PDW <unlockable> [#michele.b, 11 September 2025, LoadFromSlotID]
	//UPDWUnlockablesManager* UnlockableManager = UPDWUnlockablesManager::Get(GetWorld());
	//UnlockableManager->InitDefaultUnlockables();

	TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
}