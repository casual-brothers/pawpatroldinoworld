// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWPlayerState.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Data/PDWUnlockableSettings.h"
#include "Data/PDWGameplayStructures.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "BlueprintGameplayTagLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

void APDWPlayerState::SetIsOnVehicle(bool bValue)
{
	//We can't have this check since if we swap from one vehicle to another we are already onvehicle and won't trigger the event of swapcomplete causing minigame bugs.
	/*if (bIsOnVehicle == bValue) return;*/
	bIsOnVehicle = bValue;
	if (PDWController)
	{
		if (APDWCharacter* Char = PDWController->GetPDWPlayerState()->GetCharacterPup())
		{
			if (Char->GetAniminstance())
			{
				Char->GetAniminstance()->SetAnimationState(bIsOnVehicle ? EAnimationState::Driving : EAnimationState::Walking);
			}

			if (Char->GetCharacterMovement())
			{
				Char->GetCharacterMovement()->bOrientRotationToMovement = !bValue;
			}
		}
		PDWController->OnSwapVehicleCompleted.Broadcast(bValue, PDWController);
	}
	UPDWDataFunctionLibrary::SetIsOnVehicle(this, bIsOnVehicle);
}

bool APDWPlayerState::GetIsOnVehicle() const
{
	return bIsOnVehicle;
}

void APDWPlayerState::SetCurrentPup(const FGameplayTag& inPupTag)
{
	CurrentPupTag = inPupTag;
	UPDWDataFunctionLibrary::SetSelectedPup(this, inPupTag);
}

const FGameplayTag& APDWPlayerState::GetCurrentPup()
{
	if (!UBlueprintGameplayTagLibrary::IsGameplayTagValid(CurrentPupTag))
	{
		CurrentPupTag = FGameplayTag::RequestGameplayTag("ID.Character.Marshall");
	}
	return CurrentPupTag;
}

APawn* APDWPlayerState::GetActivePawn() const
{
	if (bIsOnVehicle)
		return VehiclePawn;
	else
		return Character;
}

void APDWPlayerState::ChangePupTreats(const int32 Difference)
{
	if (!RewardData || RewardData->ExpToLevelUp <= 0)
		return;

	const int32 oldValue = UPDWDataFunctionLibrary::GetExpPoints(this);
	const int32 newValue = oldValue + Difference;
	UPDWDataFunctionLibrary::AddExpPoints(this, Difference);

	int32 OldLevel = oldValue / RewardData->ExpToLevelUp;
	int32 NewLevel = newValue / RewardData->ExpToLevelUp;

	if (NewLevel > OldLevel)
	{
		HandleLevelUp(NewLevel);
	}
	
	OnPupTreatChanged.Broadcast(this, newValue - (RewardData->ExpToLevelUp * NewLevel), Difference);
}

void APDWPlayerState::SetPlayerStates(const FGameplayTagContainer& inContainer)
{
	PlayerStateContainer = inContainer;
}

void APDWPlayerState::AddPlayerStates(const FGameplayTagContainer& inContainer)
{
	PlayerStateContainer.AppendTags(inContainer);
}

void APDWPlayerState::RemovePlayerStates(const FGameplayTagContainer& inContainer)
{
	PlayerStateContainer.RemoveTags(inContainer);
}

void APDWPlayerState::SetMiniGameVehiclePhysicsOff(const bool bInActive)
{
	PhysicsOff = bInActive;
}

bool APDWPlayerState::GetMiniGameVehiclePhysicsOff()
{
	return PhysicsOff;
}

FGameplayTagContainer APDWPlayerState::GetPlayerStates() const
{
	return PlayerStateContainer;
}
int32 APDWPlayerState::GetExpToLevelUp()
{
	return RewardData ? RewardData->ExpToLevelUp : 0;
}

int32 APDWPlayerState::GetCurrentExp()
{
	const int32 Exp = UPDWDataFunctionLibrary::GetExpPoints(this);
	int32 Level = Exp / RewardData->ExpToLevelUp;
	return Exp - (RewardData->ExpToLevelUp * Level);
}

void APDWPlayerState::SetCharacterPup(APDWCharacter* inCHar)
{
	Character= inCHar;
}
APDWCharacter* APDWPlayerState::GetCharacterPup()
{
	return Character;
}

AActor* APDWPlayerState::GetItem(const FName& inID)
{
	return Inventory.FindAndRemoveChecked(inID);
}

void APDWPlayerState::AddItem(const FName& inID, AActor* inItem)
{
	Inventory.Add(inID,inItem);
}

void APDWPlayerState::HandleLevelUp(int32 Level)
{
	if(!RewardData || !RewardData->Reward.Contains(Level))
		return;

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	
	if (GM && GM->FSMHelper)
	{
		GM->FSMHelper->PendingRewards = RewardData->Reward[Level];
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetRewardTag().ToString(), "FromLevelUp");
	}
}

void APDWPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UPDWUnlockableSettings* UnlockableSettings = UPDWUnlockableSettings::Get();
	RewardData = UnlockableSettings->GetLevelUpSettings();
	if (GetPlayerController())
	{
		PDWController = Cast<APDWPlayerController>(GetPlayerController());
		if (PDWController)
		{
			Character = Cast<APDWCharacter>(PDWController->GetPupInstance());
		}
	}
}

