// Fill out your copyright notice in the Description page of Project Settings.

#include "Flow/States/PDWTeleportFSMState.h"
#include "Data/PDWGameSettings.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "WPHelperSubsystem.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWTeleportFSMState::OnFSMStateEnter_Implementation(const FString& InOption /* = FString("") */)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UPDWGameplayFunctionLibrary::ApplyQualitySettings(this, "Teleport");

	UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, true);
}

void UPDWTeleportFSMState::OnFSMStateExit_Implementation()
{
	CurrentPopupDialog = nullptr;

	UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, false);

	UPDWGameplayFunctionLibrary::RestoreQualitySettings(this, "Teleport");

	Super::OnFSMStateExit_Implementation();
}

void UPDWTeleportFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	if (bIsTeleporting)
	{
		return;
	}

	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);
	if (Action == UPDWGameSettings::GetUIActionConfirm())
	{
		if (UPDWDataFunctionLibrary::GetPlayerAreaId(this) == FGameplayTag::RequestGameplayTag(FName(*Parameter)))
		{
			SelectedTeleportLocation = FGameplayTag::EmptyTag;

			// player is already here!
			CurrentPopupDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, FName("PlayerIsAlreadyHere"),
				[=, this](FString InResponse)
				{
					OnPopupDismissed(InResponse);
				}
			, StateOwner);
		}
		// travel to selected location
		else 
		{
			if (!UPDWDataFunctionLibrary::IsAreaVisited(this, FGameplayTag::RequestGameplayTag(FName(*Parameter))))
			{
				// Ignore traveling to a locked location
				// TODO: Maybe add a popup message
				UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), "Negative");

				return;
			}

			const FPDWAreaInfo AreaInfo = UPDWGameSettings::GetAreaInfo(FGameplayTag::RequestGameplayTag(FName(*Parameter)));

			SelectedTeleportLocation = AreaInfo.TeleportLocationId;

			CurrentPopupDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, FName("AreYouSureTravel"),
				[=, this](FString InResponse)
				{
					OnPopupDismissed(InResponse);
				}
			, StateOwner);
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionBack())
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
	}
}

void UPDWTeleportFSMState::OnPopupDismissed(const FString& InResponse)
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, CurrentPopupDialog);

	if (InResponse == "Yes" && SelectedTeleportLocation != FGameplayTag::EmptyTag)
	{
		if (UWPHelperSubsystem* HelperSubsystem = UWPHelperSubsystem::Get(this))
		{
			if (APDWPlayerController* PlayerController = Cast<APDWPlayerController>(StateOwner))
			{
				TArray<AActor*> InActorsToTeleport{};
				InActorsToTeleport.Add(PlayerController->GetIsOnVehicle() ? PlayerController->GetVehicleInstance() : PlayerController->GetPawn());
				if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
				{
					APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
					if (PC2)
					{
						InActorsToTeleport.Add(PC2->GetIsOnVehicle() ? PC2->GetVehicleInstance() : PC2->GetPawn());
					}
				}

				UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
				if (UIManager)
				{
					UIManager->StartTransitionIn();
				}
				
				bIsTeleporting = true;

				HelperSubsystem->OnTeleportCompleted.AddUniqueDynamic(this, &ThisClass::OnTeleportComplete);
				UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, false);
				HelperSubsystem->TeleportToLocation(SelectedTeleportLocation, InActorsToTeleport);
				UPDWDataFunctionLibrary::SetPlayerAreaIdByTeleportId(this, SelectedTeleportLocation);
				UPDWEventSubsytem::Get(this)->OnTeleportConfirm.Broadcast();
				UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), OnTeleportStartAudioID);
			}
		}
	}
}

void UPDWTeleportFSMState::OnTeleportComplete()
{
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this, &ThisClass::OnTeleportComplete);
	PageRef->SetVisibility(ESlateVisibility::Collapsed);
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		UIManager->StartTransitionOut();
	}
	UPDWEventSubsytem::Get(this)->OnTransitionOutEnd.AddUniqueDynamic(this, &ThisClass::OnTransitionOutEnd);
}

void UPDWTeleportFSMState::OnTransitionOutEnd()
{
	UPDWEventSubsytem::Get(this)->OnTransitionOutEnd.RemoveDynamic(this, &ThisClass::OnTransitionOutEnd);
	TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
}