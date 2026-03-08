// Fill out your copyright notice in the Description page of Project Settings.

#include "Flow/States/PDWRyderMenuFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Data/PDWGameSettings.h"
#include "UI/Pages/PDWRyderMenuPage.h"
#include "Modes/PDWGameplayGameMode.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"


void UPDWRyderMenuFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	UPDWRyderMenuPage* RyderMenuPage = Cast<UPDWRyderMenuPage>(PageRef);
	if (!ensureMsgf(RyderMenuPage, TEXT("RyderMenuPage is of the wrong type!")))
	{
		return;
	}

	if (Action == UFlowDeveloperSettings::GetCustomizationTag().GetTagName())
	{
		TriggerTransition(UFlowDeveloperSettings::GetCustomizationTag().GetTagName());
	}
	else if (Action == UFlowDeveloperSettings::GetReplayMinigamesTag().GetTagName())
	{
		RyderMenuPage->ShowMinigames();
	}
	else if (Action == "Replay") // internal only action, specific for invoking a replay of a minigame
	{
		if(DialogInstance)
			return;

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Replay")));
		APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
		if (!ensureMsgf(GameMode, TEXT("Coulnd't retrieve GameMode trygin to replay minigame!")))
		{
			return;
		}

		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			TArray<ANebulaFlowPlayerController*> SyncControllers = TArray<ANebulaFlowPlayerController*>();
			SyncControllers.Add(GameMode->GetPlayerControllerOne());
			SyncControllers.Add(GameMode->GetPlayerControllerTwo());
			APDWPlayerController* Sender = Cast<APDWPlayerController>(ControllerSender);

			DialogInstance = UNebulaFlowUIFunctionLibrary::ShowDialog(this, "RequestToJoin", [&](FString InString) {
				OnRequestToJoinAnswered(InString, Sender);
				}, nullptr, SyncControllers);
		}
		else
		{
			ReplayMinigame();
		}
	}
	else if (Action == UPDWGameSettings::GetUIActionBack())
	{
		if (RyderMenuPage->GetCurrentMenuState() == EPDWRyderMenuState::Minigames)
		{
			RyderMenuPage->ShowSelection();
		}
		else
		{
			TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
		}
	}
}

void UPDWRyderMenuFSMState::OnRequestToJoinAnswered(FString inAnswer, APDWPlayerController* inController)
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this,DialogInstance);
	DialogInstance = nullptr;
	if (inAnswer == "Yes")
	{
		ReplayMinigame();
	}
}

void UPDWRyderMenuFSMState::ReplayMinigame()
{
	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	UPDWRyderMenuPage* RyderMenuPage = Cast<UPDWRyderMenuPage>(PageRef);
	FGameplayTagContainer MinigameTags = RyderMenuPage->GetMinigameIds();

	if (!GameMode || !RyderMenuPage || MinigameTags.IsEmpty()) return;

	GameMode->ReplayMinigame(MinigameTags);
	TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
}
