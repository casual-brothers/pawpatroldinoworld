// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWRewardFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "Data/PDWGameplayStructures.h"
#include "Data/PDWUnlockableSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"
#include "UI/Pages/PDWRewardPage.h"
#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Pawns/PDWCharacter.h"

void UPDWRewardFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	if (PageRef)
	{
		APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
		//UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, true);

		for (TSoftObjectPtr<UPDWUnlockableData> Reward : GM->FSMHelper->PendingRewards.Unlocakbles)
		{
			UPDWUnlockableData* RewardData = Reward.LoadSynchronous();

			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Unlocked: %s"), *RewardData->UnlockableName.ToString()));
			UPDWDataFunctionLibrary::AddItemToInventory(this, RewardData->ID, 1);

			if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
			{
				EventSubsystem->OnItemCollectedEvent(RewardData->ID);
			}
		}

		RewardPage = Cast<UPDWRewardPage>(PageRef);
		if (RewardPage)
		{
			RewardPage->InitRewardPage(GM->FSMHelper->PendingRewards, InOption == "FromLevelUp");
		}
	}
	else
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName(), "");
	}
}

void UPDWRewardFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UPDWGameSettings::GetUIActionConfirm())
	{
		StartEndFlow();
	}
	if (Action == UPDWGameSettings::GetCustomizationAction())
	{
		TriggerTransition(UFlowDeveloperSettings::GetCustomizationTag().GetTagName());
	}
	if (Action == UPDWGameSettings::GetEquipNowAction())
	{
		APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
		if (!(GM && GM->FSMHelper->PendingRewards.Unlocakbles.Num()))
		{
			StartEndFlow();
		}
		CustomizePup(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this), GM->FSMHelper->PendingRewards.Unlocakbles[0]->ID);
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			CustomizePup(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this), GM->FSMHelper->PendingRewards.Unlocakbles[0]->ID);
		}
		StartEndFlow();
	}
}

void UPDWRewardFSMState::StartEndFlow()
{
	if (RewardPage)
	{
		if (!RewardPage->CheckForNextUnlockable())
		{
			TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
		}
	}
}

void UPDWRewardFSMState::OnFSMStateExit_Implementation()
{
	//UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, false);
	
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	if(GM && GM->FSMHelper) GM->FSMHelper->PendingRewards = FPDWUnlockableContainer();

	Super::OnFSMStateExit_Implementation();
}

void UPDWRewardFSMState::CustomizePup(APDWPlayerController* PDWPlayerController, FGameplayTag NewCustomization)
{
	FCustomizationData CurrentCustomization = UPDWDataFunctionLibrary::GetCustomizationData(this);
	FGameplayTagContainer ContainerTags;
	ContainerTags.AddTag(NewCustomization);
	if (NewCustomization.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("ID.Collectible.Cosmetic.Pup"))))
	{
		CurrentCustomization.PupCustomizations.Emplace(PDWPlayerController->GetCurrentPup(), NewCustomization);
		UNebulaGraphicsCustomizationComponent* PupGraphicsCustomizationComp = PDWPlayerController->GetPupInstance()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>();
		PupGraphicsCustomizationComp->CustomizeStaticMesh(ContainerTags);
		PupGraphicsCustomizationComp->CustomizeTexture(ContainerTags);
	}
	else if (NewCustomization.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("ID.Collectible.Cosmetic.Vehicle"))))
	{
		CurrentCustomization.VehicleCustomizations.Emplace(PDWPlayerController->GetCurrentPup(), NewCustomization);
		UNebulaGraphicsCustomizationComponent* VehicleGraphicsCustomizationComp = PDWPlayerController->GetVehicleInstance()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>();
		VehicleGraphicsCustomizationComp->CustomizeMaterial(ContainerTags);
	}

	UPDWDataFunctionLibrary::SetCustomizationData(this, CurrentCustomization);
}