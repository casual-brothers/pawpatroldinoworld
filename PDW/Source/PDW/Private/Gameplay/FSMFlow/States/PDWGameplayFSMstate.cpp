// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWGameplayFSMstate.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Managers/PDWEventSubsytem.h"
#include "EnhancedInputSubsystems.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWPlayerState.h"
#include "UI/NebulaFlowUIConstants.h"
#include "PDWGameInstance.h"
#include "Managers/PDWInputSubsystem.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"

void UPDWGameplayFSMstate::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	//APDWPlayerController* ControllerOne = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	//if (ControllerOne)
	//{
	//	ControllerOne->AddMovementAbilities();
	//}
	//if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	//{
	//	APDWPlayerController* ControllerTwo = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
	//	if (ControllerTwo)
	//	{
	//		ControllerTwo->AddMovementAbilities();
	//	}
	//}

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnGameplayStateEnterEvent();
	}

	UNebulaFlowUIFunctionLibrary::SetFocusToGameViewport(this);

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	
	if (GM && GM->FSMHelper && GM->FSMHelper->PendingGameplayAction.Action != "")
	{
		FPDWTriggerActionData ActionData = GM->FSMHelper->PendingGameplayAction;
		GM->FSMHelper->PendingGameplayAction = {};
		OnFSMStateAction_Implementation(ActionData.Action, ActionData.Parameter, ActionData.ControllerSender);
	}
	if (GM && GM->FSMHelper && GM->FSMHelper->PendingRewards.Unlocakbles.Num() > 0)
	{
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetRewardTag().ToString(), "");
	}

	IsClosingCharacterSelectorByPlayer.Add(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this), false);
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		IsClosingCharacterSelectorByPlayer.Add(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this), false);
	}
}

void UPDWGameplayFSMstate::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UFlowDeveloperSettings::GetRyderMenuTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetRyderMenuTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetTeleportTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetTeleportTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetCutsceneTag().ToString())
	{
		TriggerTransitionWithOption(UFlowDeveloperSettings::GetCutsceneTag().GetTagName(), Parameter);
	}
	if (Action == UFlowDeveloperSettings::GetDialogueTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetDialogueTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetMinigameSetupTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetMinigameSetupTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetRewardTag().ToString())
	{
		TriggerTransitionWithOption(UFlowDeveloperSettings::GetRewardTag().GetTagName(), Parameter);
	}	
	if (Action == UFlowDeveloperSettings::GetPauseGameTag().ToString())
	{
		TriggerTransitionWithOption(UFlowDeveloperSettings::GetPauseGameTag().GetTagName(), UPDWGameplayFunctionLibrary::IsSecondPlayer(ControllerSender) ? "1" : "0");
	}
	if (Action == UFlowDeveloperSettings::GetQuestMessageTag().GetTagName())
	{
		TriggerTransition(UFlowDeveloperSettings::GetQuestMessageTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetCustomizationTag().GetTagName())
	{
		if (IsClosingCharacterSelectorByPlayer.Contains(ControllerSender) && IsClosingCharacterSelectorByPlayer[ControllerSender])
		{
			return;
		}
		UPDWHUDSubsystem::Get(this)->RequestToggleSwapCharacter(ControllerSender, false);
		TriggerTransition(UFlowDeveloperSettings::GetCustomizationTag().GetTagName());
	}
	if (Action == UFlowDeveloperSettings::GetDinoCustomizationTag().GetTagName())
	{
		APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
		GM->FSMHelper->PendingDinoTagCustomization = FGameplayTag::RequestGameplayTag(FName(Parameter));
		TriggerTransitionWithOption(UFlowDeveloperSettings::GetDinoCustomizationTag().GetTagName(), UPDWGameplayFunctionLibrary::IsSecondPlayer(ControllerSender) ? "1" : "0");
	}

#pragma region SwapCharacterWidget
	if (Action == UPDWGameSettings::GetActionChangePup())
	{
		HandleSkillsOnCHangeState(ControllerSender);
		UPDWInputSubsystem::ApplyMappingContextByTag(UPDWGameSettings::GetUIDefaultTagMap(), ControllerSender);
		UPDWHUDSubsystem::Get(this)->RequestToggleSwapCharacter(ControllerSender, true);
	}

	if (Action == UPDWGameSettings::GetUIActionConfirm())
	{
		UPDWHUDSubsystem::Get(this)->RequestToggleSwapCharacter(ControllerSender, false, true);
		if (IsClosingCharacterSelectorByPlayer.Contains(ControllerSender))
		{
			IsClosingCharacterSelectorByPlayer[ControllerSender] = true;
		}
	}

	if (Action == UPDWGameSettings::GetActionBackToGameplay())
	{
		RemoveGameplayEffectOnUIInteraction(ControllerSender);
		UPDWInputSubsystem::ApplyMappingContextByTag(UPDWGameSettings::GetDefaultGameplayTagMap(), ControllerSender);
		UNebulaFlowUIFunctionLibrary::SetFocusToGameViewport(this);
		if (IsClosingCharacterSelectorByPlayer.Contains(ControllerSender))
		{
			IsClosingCharacterSelectorByPlayer[ControllerSender] = false;
		}
	}

	if (Action == UPDWGameSettings::GetActionCloseSwapCharacter())
	{
		UPDWHUDSubsystem::Get(this)->RequestToggleSwapCharacter(ControllerSender, false);
		if (IsClosingCharacterSelectorByPlayer.Contains(ControllerSender))
		{
			IsClosingCharacterSelectorByPlayer[ControllerSender] = true;
		}
	}
#pragma endregion

}

void UPDWGameplayFSMstate::OnFSMStateExit_Implementation()
{
	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnGameplayStateExitEvent();
	}
	HandleSkillsOnCHangeState();
	UPDWGameplayFunctionLibrary::StopForceFeedbackBothPlayers(this);
	Super::OnFSMStateExit_Implementation();
}

void UPDWGameplayFSMstate::HandleSkillsOnCHangeState(APlayerController* inController /*= nullptr*/)
{
	if (inController)
	{
		APDWPlayerController* CurrentController = Cast<APDWPlayerController>(inController);
		if(!CurrentController)
			return;
		UPDWGASComponent* GasComp = CurrentController->GetGASComponent();
		if (GasComp)
		{
			FGameplayEffectContextHandle EffectContext = GasComp->MakeEffectContext();
			EffectContext.AddSourceObject(GasComp->GetOwner());
			FGameplayEffectSpecHandle NewHandle = GasComp->MakeOutgoingSpec(CurrentController->DisableSkillEffect,0, EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = GasComp->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), GasComp);
			}
		}
	}
	else
	{

		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			UPDWGASComponent* GasComp = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this)->GetGASComponent();
			if (GasComp)
			{
				FGameplayEffectContextHandle EffectContext = GasComp->MakeEffectContext();
				EffectContext.AddSourceObject(GasComp->GetOwner());
				FGameplayEffectSpecHandle NewHandle = GasComp->MakeOutgoingSpec(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this)->DisableSkillEffect, 0, EffectContext);
				if (NewHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = GasComp->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), GasComp);
				}
			}
		}
		UPDWGASComponent* GasComp = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->GetGASComponent();
		if (GasComp)
		{
			FGameplayEffectContextHandle EffectContext = GasComp->MakeEffectContext();
			EffectContext.AddSourceObject(GasComp->GetOwner());
			
			FGameplayEffectSpecHandle NewHandle = GasComp->MakeOutgoingSpec(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->DisableSkillEffect, 0, EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = GasComp->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), GasComp);
			}
		}
	}
}

void UPDWGameplayFSMstate::RemoveGameplayEffectOnUIInteraction(APlayerController* inController)
{
	if (inController)
	{
		APDWPlayerController* CurrentController = Cast<APDWPlayerController>(inController);
		if(!CurrentController)
			return;
		UPDWGASComponent* GasComp = CurrentController->GetGASComponent();
		if (GasComp)
		{
			GasComp->RemoveActiveGameplayEffectBySourceEffect(CurrentController->DisableSkillEffect,GasComp);
		}
	}
}
