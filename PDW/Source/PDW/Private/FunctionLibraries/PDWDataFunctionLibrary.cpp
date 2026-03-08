// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/PDWLocalPlayer.h"
#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "FlowSubsystem.h"
#if WITH_EDITOR
#include "ToxicUtilitiesSetting.h"
#endif
#include "Data/PDWGameSettings.h"

UPDWPersistentUser* UPDWDataFunctionLibrary::GetPersistentUser(UObject* WorldContextObject)
{
	UNebulaFlowLocalPlayer* PlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject);
		if(PlayerOwner)
		{
	#if WITH_EDITOR
			if (!Cast<UPDWPersistentUser>(PlayerOwner->GetPersistentUser()))
			{				
				//To Avoid Crash in PIE	
				PlayerOwner->LoadPersistentUser();
				UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
				if (CurrentUser)
				{
					CurrentUser->OnProfilePostLoad();
				}
			}
	#endif
			return Cast<UPDWPersistentUser>(PlayerOwner->GetPersistentUser());
		}

	return nullptr;
}

bool UPDWDataFunctionLibrary::GetIsNewGame(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.SlotMetaData.IsNewGame;
	}

	return true;
}

void UPDWDataFunctionLibrary::SetIsNewGame(const bool IsNewGame, UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.SlotMetaData.IsNewGame = IsNewGame;
	}
}

bool UPDWDataFunctionLibrary::GetIsFirstBoot(UObject* WorldContextObject)
{
	//TODO
	return true;
}

void UPDWDataFunctionLibrary::SetIsFirstBoot(UObject* WorldContextObject,const bool IsFirstBoot)
{
	//TODO
}

FTransform UPDWDataFunctionLibrary::GetPlayerPosition(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.PlayerPosition;
	}

	return FTransform();
}

void UPDWDataFunctionLibrary::SetPlayerPosition(UObject* WorldContextObject, const FTransform& Position)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.PlayerPosition = Position;
	}
}

FGameplayTag UPDWDataFunctionLibrary::GetSelectedPup(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.SelectedPup;
	}
	return FGameplayTag();
}

void UPDWDataFunctionLibrary::SetSelectedPup(UObject* WorldContextObject,const FGameplayTag& Pup)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.SelectedPup = Pup;
	}
}

bool UPDWDataFunctionLibrary::GetIsOnVehicle(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.IsPupOnVechicle;
	}

	return false;
}

void UPDWDataFunctionLibrary::SetIsOnVehicle(UObject* WorldContextObject,const bool IsOnVehicle)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.IsPupOnVechicle = IsOnVehicle;
	}
}

int32 UPDWDataFunctionLibrary::GetExpPoints(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.ExpPoints;
	}

	return 0;
}

FCustomizationData UPDWDataFunctionLibrary::GetCustomizationData(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.CustomizationData;
	}

	return FCustomizationData();
}

void UPDWDataFunctionLibrary::SetCustomizationData(UObject* WorldContextObject, FCustomizationData CustomizationData)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.CustomizationData = CustomizationData;
	}
}

void UPDWDataFunctionLibrary::AddExpPoints(UObject* WorldContextObject,const int32 AddedPoints)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.ExpPoints += AddedPoints;
	}
}

bool UPDWDataFunctionLibrary::GetFact(UObject* WorldContextObject,const FGameplayTag& Fact)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.Facts.Contains(Fact))
		{
			return false;
		}
		else
		{
			return SavedData.WorldData.Facts[Fact];
		}
	}

	return true;
}

void UPDWDataFunctionLibrary::SetInteractableState(UObject* WorldContextObject, const int32 InteractableID, const FInteractableData& InteractableData)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.QuestInteractableStates.Emplace(InteractableID, InteractableData);
	}
}

FInteractableData UPDWDataFunctionLibrary::GetInteractableState(UObject* WorldContextObject, const int32 InteractableID)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (SavedData.WorldData.QuestInteractableStates.Contains(InteractableID))
		{
			return SavedData.WorldData.QuestInteractableStates[InteractableID];
		}
	}

	return FInteractableData();
}

void UPDWDataFunctionLibrary::RemoveInteractableStateByIdentityTags(UObject* WorldContextObject, const FGameplayTagContainer IdentityTags)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		TArray<int32> KeyArray = {};
		SavedData.WorldData.QuestInteractableStates.GenerateKeyArray(KeyArray);

		for (int32 i = KeyArray.Num() - 1; i >= 0; i--)
		{
			if (SavedData.WorldData.QuestInteractableStates[KeyArray[i]].IdentityTags.HasAllExact(IdentityTags))
			{
				SavedData.WorldData.QuestInteractableStates.Remove(KeyArray[i]);
			}
		}
	}
}

void UPDWDataFunctionLibrary::RemoveInteractableStateByIndex(UObject* WorldContextObject, const int32 InteractableID)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.QuestInteractableStates.Remove(InteractableID);		
	}
}

void UPDWDataFunctionLibrary::AddUsedInteractable(UObject* WorldContextObject, const int32 InteractableID)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.UsedInteractables.AddUnique(InteractableID);
	}
}

bool UPDWDataFunctionLibrary::IsInteractableUsed(UObject* WorldContextObject, int32 InteractableID)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.UsedInteractables.Contains(InteractableID);
	}

	return false;
}

int32 UPDWDataFunctionLibrary::GetUsedInteractableCount(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.UsedInteractables.Num();
	}

	return -1;
}

TArray<FGameplayTag> UPDWDataFunctionLibrary::GetVisitedAreaIds(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.VisitedAreaIds;
	}

	return {};
}

bool UPDWDataFunctionLibrary::IsAreaVisited(UObject* WorldContextObject, const FGameplayTag& RequestedAreaId)
{
	TArray<FGameplayTag> VisitedAreas = GetVisitedAreaIds(WorldContextObject);
	return VisitedAreas.Contains(RequestedAreaId);
}

FGameplayTag UPDWDataFunctionLibrary::GetPlayerAreaId(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.LastVisitedAreaId;
	}

	return FGameplayTag::EmptyTag;
}

void UPDWDataFunctionLibrary::SetPlayerAreaId(UObject* WorldContextObject, const FGameplayTag& CurrentAreaId)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (CurrentAreaId != FGameplayTag::EmptyTag)
		{
			SavedData.PlayerData.LastVisitedAreaId = CurrentAreaId;
		}
		else if (SavedData.PlayerData.LastVisitedAreaId == FGameplayTag::EmptyTag)
		{
			SavedData.PlayerData.LastVisitedAreaId = FGameplayTag::RequestGameplayTag("Map.DinoPlains");
		}
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, *FString::Printf(TEXT("Set area Id:%s"), *SavedData.PlayerData.LastVisitedAreaId.ToString()));

		SavedData.PlayerData.VisitedAreaIds.AddUnique(CurrentAreaId);
	}
}

void UPDWDataFunctionLibrary::SetPlayerAreaIdByTeleportId(UObject* WorldContextObject, const FGameplayTag& TeleportId)
{
	TMap<FGameplayTag, FPDWAreaInfo> Areas = UPDWGameSettings::GetAllAreaInfo();
	for (const TPair<FGameplayTag, FPDWAreaInfo>& Pair : Areas)
	{
		const FGameplayTag& AreaTag = Pair.Key;
		const FPDWAreaInfo& AreaInfo = Pair.Value;
		if (AreaInfo.TeleportLocationId == TeleportId)
		{
			SetPlayerAreaId(WorldContextObject, AreaTag);
			return;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, *FString::Printf(TEXT("Cannot find Area Id associated to this teleport Id")));
	
}

TArray<FQuestData> UPDWDataFunctionLibrary::GetCompletedQuests(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.CompletedQuests;
	}

	return TArray<FQuestData>();
}

void UPDWDataFunctionLibrary::AddCompletedQuest(UObject* WorldContextObject, FQuestData CompletedQuest)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		CompletedQuest.IsCompleted = true;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.CompletedQuests.Add(CompletedQuest);
	}
}

TArray<FPDWMinigameInfo> UPDWDataFunctionLibrary::GetPlayedMinigamesInfo(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.CompletedMinigames;
	}

	return TArray<FPDWMinigameInfo>();
}

void UPDWDataFunctionLibrary::AddPlayedMinigame(UObject* WorldContextObject, FPDWMinigameInfo PlayedMinigame)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.CompletedMinigames.AddUnique(PlayedMinigame);
	}
}

void UPDWDataFunctionLibrary::SetCurrentQuestDescription(UObject* WorldContextObject, const FText& QuestDescription)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.CurrentQuestDescription = QuestDescription;
	}
}

void UPDWDataFunctionLibrary::IncrementPupTreatsCollected(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.AchievementsData.PupTreatsCollected++;
	}
}

int32 UPDWDataFunctionLibrary::GetPupTreatsCollected(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.AchievementsData.PupTreatsCollected;
	}

	return -1;
}

void UPDWDataFunctionLibrary::IncrementDinoNeedsSolved(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.AchievementsData.DinoNeedsSolved++;
	}
}

int32 UPDWDataFunctionLibrary::GetDinoNeedsSolved(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.AchievementsData.DinoNeedsSolved;
	}

	return -1;
}

void UPDWDataFunctionLibrary::IncrementOpenedChests(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.AchievementsData.OpenedChests++;
	}
}

int32 UPDWDataFunctionLibrary::GetOpenedChests(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.AchievementsData.OpenedChests;
	}

	return -1;
}

void UPDWDataFunctionLibrary::AddDisabledInput(UObject* WorldContextObject, FName InputAction)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.PlayerData.DisabledInputActions.Contains(InputAction))
		{
			SavedData.PlayerData.DisabledInputActions.Add(InputAction);
		}
	}
}

void UPDWDataFunctionLibrary::RemoveDisabledInput(UObject* WorldContextObject, FName InputAction)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.DisabledInputActions.Remove(InputAction);
	}
}

TSet<FName> UPDWDataFunctionLibrary::GetDisabledInputs(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.DisabledInputActions;
	}

	return TSet<FName>();
}

void UPDWDataFunctionLibrary::SetAllowedPups(UObject* WorldContextObject, FGameplayTagContainer AllowedPups)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.AllowedPups = AllowedPups;
	}
}

void UPDWDataFunctionLibrary::ClearAllowedPups(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.AllowedPups = FGameplayTagContainer();
	}
}

FGameplayTagContainer UPDWDataFunctionLibrary::GetAllowedPups(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.AllowedPups;
	}

	return FGameplayTagContainer();
}

TArray<int32> UPDWDataFunctionLibrary::GetCollectedTreats(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.CollectedTreats;
	}

	return TArray<int32>();
}

void UPDWDataFunctionLibrary::AddCollectedTreat(UObject* WorldContextObject,const int32 TreatId)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.CollectedTreats.AddUnique(TreatId);
	}
}

void UPDWDataFunctionLibrary::ChangeDinoPenActivity(UObject* WorldContextObject, FGameplayTag DinoPenTag, bool IsActive)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.DinoPensData.Contains(DinoPenTag))
		{
			SavedData.WorldData.DinoPensData.Add(DinoPenTag);
		}

		SavedData.WorldData.DinoPensData[DinoPenTag].IsActive = IsActive;		
	}
}

bool UPDWDataFunctionLibrary::IsDinoPenActive(UObject* WorldContextObject, FGameplayTag DinoPenTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (SavedData.WorldData.DinoPensData.Contains(DinoPenTag))
		{
			return SavedData.WorldData.DinoPensData[DinoPenTag].IsActive;
		}
	}

	return false;
}

FGameplayTag UPDWDataFunctionLibrary::GetDinoNeed(UObject* WorldContextObject, FGameplayTag DinoPenTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.DinoPensData.Contains(DinoPenTag) ? SavedData.WorldData.DinoPensData[DinoPenTag].CurrentNeed : FGameplayTag();
	}

	return FGameplayTag();
}

FGameplayTag UPDWDataFunctionLibrary::GetFakeDinoNeed(UObject* WorldContextObject, FGameplayTag DinoPenTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.DinoPensData.Contains(DinoPenTag) ? SavedData.WorldData.DinoPensData[DinoPenTag].CurrentFakeNeed : FGameplayTag();
	}

	return FGameplayTag();
}

void UPDWDataFunctionLibrary::SetDinoNeed(UObject* WorldContextObject, FGameplayTag DinoPenTag, FGameplayTag DinoNeed)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.DinoPensData.Contains(DinoPenTag))
		{
			SavedData.WorldData.DinoPensData.Add(DinoPenTag);
		}
					
		SavedData.WorldData.DinoPensData[DinoPenTag].CurrentNeed = DinoNeed;
	}
}

void UPDWDataFunctionLibrary::SetFakeDinoNeed(UObject* WorldContextObject, FGameplayTag DinoPenTag, FGameplayTag DinoNeed)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.DinoPensData.Contains(DinoPenTag))
		{
			SavedData.WorldData.DinoPensData.Add(DinoPenTag);
		}
					
		SavedData.WorldData.DinoPensData[DinoPenTag].CurrentFakeNeed = DinoNeed;
	}
}

FGameplayTag UPDWDataFunctionLibrary::GetDinoCustomization(UObject* WorldContextObject, FGameplayTag DinoPenTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.DinoPensData.Contains(DinoPenTag) ? SavedData.WorldData.DinoPensData[DinoPenTag].DinoCustomization : FGameplayTag();
	}

	return FGameplayTag();
}

void UPDWDataFunctionLibrary::SetDinoCustomization(UObject* WorldContextObject, FGameplayTag DinoPenTag, FGameplayTag CustomizationTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.DinoPensData.Contains(DinoPenTag))
		{
			SavedData.WorldData.DinoPensData.Add(DinoPenTag);
		}
					
		SavedData.WorldData.DinoPensData[DinoPenTag].DinoCustomization = CustomizationTag;
	}
}

void UPDWDataFunctionLibrary::AddItemToInventory(UObject* WorldContextObject, const FGameplayTag ItemTag, int32 Quantity)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.PlayerData.Inventory.Contains(ItemTag))
		{
			SavedData.PlayerData.Inventory.Add(ItemTag,0);
		}

		SavedData.PlayerData.Inventory[ItemTag] += Quantity;
	}
}

void UPDWDataFunctionLibrary::UpdateItemQuantity(UObject* WorldContextObject, const FGameplayTag ItemTag, int32 Quantity)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (SavedData.PlayerData.Inventory.Contains(ItemTag))
		{
			SavedData.PlayerData.Inventory[ItemTag] = Quantity;	
		}
	}
}

bool UPDWDataFunctionLibrary::RemoveItemFromInventory(UObject* WorldContextObject, const FGameplayTag ItemTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.Inventory.Remove(ItemTag) > 0;
	}

	return false;
}

bool UPDWDataFunctionLibrary::RemoveItemQuantityFromInventory(UObject* WorldContextObject, const FGameplayTag ItemTag, int32 Quantity)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (SavedData.PlayerData.Inventory.Contains(ItemTag))
		{
			SavedData.PlayerData.Inventory[ItemTag] -= Quantity;
			if (SavedData.PlayerData.Inventory.Num() <= 0)
			{
				SavedData.PlayerData.Inventory.Remove(ItemTag);
			}
		}
	}

	return false;
}

int32 UPDWDataFunctionLibrary::GetItemQuantityFromInventory(UObject* WorldContextObject, const FGameplayTag ItemTag)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (SavedData.PlayerData.Inventory.Contains(ItemTag))
		{
			return SavedData.PlayerData.Inventory[ItemTag];
		}
	}

	return 0;
}

TMap<FGameplayTag, int32> UPDWDataFunctionLibrary::GetInventory(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.PlayerData.Inventory;
	}

	return TMap<FGameplayTag, int32>();
}

TMap<FGameplayTag, int32> UPDWDataFunctionLibrary::GetItemsByFilter(UObject* WorldContextObject, const FGameplayTag Filter)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	TMap<FGameplayTag, int32> Result = {};

	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		for(TPair<FGameplayTag,int32> Item : SavedData.PlayerData.Inventory)
		{
			if (Item.Key.MatchesTag(Filter))
			{
				Result.Add(Item);
			}
		}

		return Result;
	}

	return TMap<FGameplayTag, int32>();
}

void UPDWDataFunctionLibrary::SetDataLayerState(UObject* WorldContextObject,TObjectPtr<UDataLayerAsset> DL, bool IsActive)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (SavedData.WorldData.DLData.Contains(DL))
		{
			SavedData.WorldData.DLData[DL] = IsActive;
		}
		else
		{
			SavedData.WorldData.DLData.Add(DL, IsActive);
		}
	}
}

 TMap<TObjectPtr<UDataLayerAsset>, bool> UPDWDataFunctionLibrary::GetDataLayersState(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.DLData;
	}

	return TMap<TObjectPtr<UDataLayerAsset>, bool>();
}

float UPDWDataFunctionLibrary::GetStationFood(UObject* WorldContextObject, const FGameplayTag DinoPen)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.DinoPensData.Contains(DinoPen) ? SavedData.WorldData.DinoPensData[DinoPen].FoodInStation : 0;
	}
	
	return 0;
}

float UPDWDataFunctionLibrary::GetStationWater(UObject* WorldContextObject, const FGameplayTag DinoPen)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		return SavedData.WorldData.DinoPensData.Contains(DinoPen) ? SavedData.WorldData.DinoPensData[DinoPen].WaterInStation : 0;
	}
	
	return 0;
}

void UPDWDataFunctionLibrary::UpdateFoodInStation(UObject* WorldContextObject, const FGameplayTag DinoPen, int32 NewQuantity)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.DinoPensData.Contains(DinoPen))
		{
			SavedData.WorldData.DinoPensData.Add(DinoPen);
		}
		
		SavedData.WorldData.DinoPensData[DinoPen].FoodInStation = NewQuantity;
	}
}

void UPDWDataFunctionLibrary::UpdateWaterInStation(UObject* WorldContextObject, const FGameplayTag DinoPen, float NewQuantity)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		if (!SavedData.WorldData.DinoPensData.Contains(DinoPen))
		{
			SavedData.WorldData.DinoPensData.Add(DinoPen);
		}
	
		SavedData.WorldData.DinoPensData[DinoPen].WaterInStation = NewQuantity;
	}
}

void UPDWDataFunctionLibrary::SetFact(UObject* WorldContextObject,const FGameplayTag& Fact,const bool bValue)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.WorldData.Facts.Emplace(Fact, bValue);
	}
}

UFlowSaveGame* UPDWDataFunctionLibrary::GetFlowSaveGame(UObject* WorldContextObject)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		UFlowSaveGame* FlowSaveGame = NewObject<UFlowSaveGame>();
		FlowSaveGame->FlowComponents = SavedData.FlowData.FlowComponents;
		FlowSaveGame->FlowInstances = SavedData.FlowData.FlowInstances;
		return FlowSaveGame;
	}

	return nullptr;

	//return GetSaveGame(WorldContextObject)->GetFlowSaveGame();
}

void UPDWDataFunctionLibrary::SetFlowSaveGame(UObject* WorldContextObject, UFlowSaveGame* FlowSaveGame)
{
	UPDWPersistentUser* SaveGame = GetPersistentUser(WorldContextObject);
	if(SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.FlowData.FlowComponents = FlowSaveGame->FlowComponents;
		SavedData.FlowData.FlowInstances = FlowSaveGame->FlowInstances;
	}
	
	//GetSaveGame(WorldContextObject)->SetFlowSaveGame(FlowSaveGame);
}

bool UPDWDataFunctionLibrary::SaveGame(UObject* WorldContext)
{
	#if WITH_EDITOR
	if(UToxicUtilitiesSetting::Get()->bDisableSavingSystem)
	{
		return false;
	}
	#endif
	UPDWLocalPlayer* LocalPlayer = Cast<UPDWLocalPlayer>(UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContext));
	if (!LocalPlayer)
		return false;
		
	UPDWPersistentUser* PersistentUser = UPDWDataFunctionLibrary::GetPersistentUser(WorldContext);
	if (!PersistentUser)
		return false;

	PersistentUser->OnBeforeAutoSave();
	bool bResult;
	LocalPlayer->SavePersistentUser(bResult);
		
	return bResult;
}