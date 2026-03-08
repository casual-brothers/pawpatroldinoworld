// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWUIManager.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/PDWInputSubsystem.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/NebulaFlowUIConstants.h"
#include "Core/NebulaFlowPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/Areas/PDWAutoSwapArea.h"



void UPDWUIManager::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	Super::InitManager(InstanceOwner);
	TArray<FGameplayTag> SavedAllowedPups = UPDWDataFunctionLibrary::GetAllowedPups(this).GetGameplayTagArray();
	CurrentAllowedPupsP1 = SavedAllowedPups.Num() > 0 ? UPDWDataFunctionLibrary::GetAllowedPups(this).GetGameplayTagArray() : DefaultAllowedPups;
	CurrentAllowedPupsP2 = CurrentAllowedPupsP1;
	UPDWEventSubsytem::Get(this)->OnPlayerAreaChange.AddUniqueDynamic(this, &ThisClass::OnPlayerAreaChange);
}

void UPDWUIManager::UninitManager()
{
	UPDWEventSubsytem::Get(this)->OnPlayerAreaChange.RemoveDynamic(this, &ThisClass::OnPlayerAreaChange);
	Super::UninitManager();
}

void UPDWUIManager::SetWidgetsHUDtoHide(EHudWidgetNames WidgetName, bool Add /*= true*/)
{
	if (Add)
	{
		WidgetsHUDToHide.Add(WidgetName);
	}
	else
	{
		WidgetsHUDToHide.Remove(WidgetName);
	}
}

const EDialogType UPDWUIManager::CurrentModalIsTypeOf(const UNebulaFlowDialog* ModalToCheck)
{
	if (CurrentGameDialog == ModalToCheck)
	{
		return EDialogType::EGameDialog;
	}
	else if (CurrentPopup == ModalToCheck)
	{
		return EDialogType::EPopupDialog;
	}
	else if (CurrentSystemDialog == ModalToCheck)
	{
		return EDialogType::ESystemDialog;
	}

	return EDialogType::EGameDialog;
}

UNebulaFlowDialog* UPDWUIManager::ShowDialog(UGameInstance* GInstance, APlayerController* Owner, FName DialogID, TFunction<void(FString) > InCallBack, TArray<ANebulaFlowPlayerController*> SyncControllers /*=*/, TArray<FText> Params /*=*/, UTexture2D* Image /*= nullptr*/)
{
	UNebulaFlowDialog* Result = nullptr;
	UNebulaFlowUIConstants* UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(GInstance);
	if (UIConstants)
	{
		const UDataTable* DialogConfTable = UIConstants->GetDialogConfigurationsTable();
		if (DialogConfTable)
		{
			FDialogConfigTableRow* Row = DialogConfTable->FindRow<FDialogConfigTableRow>(DialogID, FString("GENERAL"));
			if (Row)
			{
				if (Row->DialogConfiguration.DialogType != EDialogType::EPopupDialog)
				{
					if (!(CurrentGameDialog || CurrentSystemDialog))
					{
						for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
						{
							if (APlayerController* PC = Iterator->Get())
							{
								UPDWInputSubsystem::ToggleUIMap(true, PC);
								if (Row->DialogConfiguration.DialogType != EDialogType::ESystemDialog)
								{
									if (SyncControllers.Num() <= 0)
									{
										SyncControllers.Add(Cast<ANebulaFlowPlayerController>(PC));
									}
								}
							}
						}

					}
				}

				if (Result && Row->DialogConfiguration.DialogType == EDialogType::EGameDialog)
				{
					UPDWHUDSubsystem::Get(this)->ManageHUD(EHUDControls::Hide);
					UNebulaFlowUIFunctionLibrary::SetUserFocus(this, Result, Owner);
				}
				Result = Super::ShowDialog(GInstance, Owner, DialogID, InCallBack, SyncControllers, Params, Image);
			}
		}
	}

	return Result;
}

bool UPDWUIManager::HideDialog(UGameInstance* GInstance, UNebulaFlowDialog* DialogToClose)
{
	bool Result = false;
	if (CurrentPopup == DialogToClose)
	{
		return Super::HideDialog(GInstance, DialogToClose);
	}
	if (CurrentGameDialog == DialogToClose || CurrentSystemDialog == DialogToClose)
	{
		Result = Super::HideDialog(GInstance, DialogToClose);
		if (CurrentGameDialog || CurrentSystemDialog)
		{
			return Result;
		}
		else
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				if (APlayerController* PC = Iterator->Get())
				{
					UPDWInputSubsystem::RestoreLastConfiguration(PC);
				}
			}

			UPDWHUDSubsystem::Get(this)->ManageHUD(EHUDControls::Show);
		}
	}

	return Result;
}

void UPDWUIManager::OnPlayerAreaChange(const APDWPlayerController* Player, FGameplayTagContainer AllowedPups, bool EnterArea, const APDWAutoSwapArea* NewArea)
{
	if (UPDWGameplayFunctionLibrary::IsSecondPlayer(Player))
	{
		if (EnterArea)
		{
			ChangeAllowedCharacterList(CurrentAllowedPupsP2, NewArea->GetDefaultAllowedPup().GetSingleTagContainer());
		}
		else
		{
			CurrentAllowedPupsP2 = DefaultAllowedPups;
		}
	}
	else
	{
		if (EnterArea)
		{
			ChangeAllowedCharacterList(CurrentAllowedPupsP1, NewArea->GetDefaultAllowedPup().GetSingleTagContainer());
		}
		else
		{
			CurrentAllowedPupsP1 = DefaultAllowedPups;
		}
	}
	
	OnAllowedPupsChanged.Broadcast();
}

void UPDWUIManager::ChangeAllowedCharacterList(TArray<FGameplayTag>& ListToChange, FGameplayTagContainer NewAllowedPups)
{
	ListToChange.Empty();
	for (FGameplayTag PupTag : DefaultAllowedPups)
	{
		if (NewAllowedPups.HasTag(PupTag))
		{
			ListToChange.Add(PupTag);
		}
	}
}

void UPDWUIManager::ChangeAllowedPups(FGameplayTagContainer NewAllowedPups, FGameplayTagContainer HighlightedPups, bool SaveIt)
{
	if (SaveIt)
	{
		UPDWDataFunctionLibrary::SetAllowedPups(this, NewAllowedPups);
	}
	ChangeAllowedCharacterList(CurrentAllowedPupsP1, NewAllowedPups);
	ChangeAllowedCharacterList(CurrentAllowedPupsP2, NewAllowedPups);
	ChangeAllowedCharacterList(CurrentHighlightedPups, HighlightedPups);
	OnAllowedPupsChanged.Broadcast();
}

void UPDWUIManager::ResetAllowedPups(bool SaveIt)
{
	if (SaveIt)
	{
		UPDWDataFunctionLibrary::ClearAllowedPups(this);
	}

	CurrentAllowedPupsP1 = DefaultAllowedPups;
	CurrentAllowedPupsP2 = DefaultAllowedPups;
	CurrentHighlightedPups.Empty();
	OnAllowedPupsChanged.Broadcast();
}

void UPDWUIManager::StartTransitionIn()
{
	if ((!CurrentTransitionWidget) && TransitionWidgetClass)
	{
		CurrentTransitionWidget = CreateWidget<UPDWTransitionWidget>(GetWorld(), TransitionWidgetClass);
		CurrentTransitionWidget->AddToViewport(9);
		CurrentTransitionWidget->StartTransitionIn();

		UPDWInputSubsystem::RemoveInputs(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this));
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			UPDWInputSubsystem::RemoveInputs(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this));
		}
	}
}

void UPDWUIManager::ShowTransitionImmediatly()
{
	if ((!CurrentTransitionWidget) && TransitionWidgetClass)
	{
		CurrentTransitionWidget = CreateWidget<UPDWTransitionWidget>(GetWorld(), TransitionWidgetClass);
		CurrentTransitionWidget->AddToViewport(9);
		
		UPDWInputSubsystem::RemoveInputs(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this));
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			UPDWInputSubsystem::RemoveInputs(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this));
		}
	}
}

void UPDWUIManager::StartTransitionOut()
{
	if (CurrentTransitionWidget)
	{
		CurrentTransitionWidget->StartTransitionOut();
		CurrentTransitionWidget = nullptr;

		UPDWInputSubsystem::RestoreLastConfiguration(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this));
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			UPDWInputSubsystem::RestoreLastConfiguration(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this));
		}
	}
}

void UPDWUIManager::RemoveTransition()
{
	if(CurrentTransitionWidget)
	{
		CurrentTransitionWidget->RemoveFromParent();
		CurrentTransitionWidget = nullptr;
	}
}

bool UPDWUIManager::IsTransitionOn()
{
	if (CurrentTransitionWidget)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UPDWUIManager::IsTransitionOnWithScreenBlack()
{
	if (CurrentTransitionWidget)
	{
		return CurrentTransitionWidget->IsScreenBlack();
	}
	
	return false;
}
