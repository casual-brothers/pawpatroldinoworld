#include "Data/PDWPersistentUser.h"

#include "Core/NebulaFlowLocalPlayer.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

const int UPDWPersistentUser::GAMEPROGRESSION_SLOTS_NUM = 3;

UPDWPersistentUser::UPDWPersistentUser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SaveGameCurrentVersion = 1; //Change after every Savegame mod - DO NOT CHANGE AFTER SUBMISSION!!!
}

UPDWPersistentUser* UPDWPersistentUser::GetCurrentOwnerUser(UObject* WorldContextObject)
{
	UNebulaFlowLocalPlayer* LocalPlayer = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject);
	if (LocalPlayer)
	{
		return Cast<UPDWPersistentUser>(LocalPlayer->GetPersistentUser());
	}
	return nullptr;
}

FGameProgressionSlot& UPDWPersistentUser::GetCurrentUsedProgressionSlot(bool& Success)
{
	Success = false;
	if (CurrentUsedSlotIndex >= 0)
	{
		return GetGameProgressionSlotByIndex(CurrentUsedSlotIndex, Success);
	}
	return GameProgressionSlots[0];
}

FGameProgressionSlot& UPDWPersistentUser::GetGameProgressionSlotByIndex(int inIndex, bool& Success)
{
	Success = false;

	if (inIndex < GAMEPROGRESSION_SLOTS_NUM)
	{
		for (int Index = 0; Index < GameProgressionSlots.Num(); Index++)
		{
			if (GameProgressionSlots[Index].SlotMetaData.SlotIndex == inIndex)
			{
				Success = true;
				return GameProgressionSlots[Index];
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No SlotFound at Index, returning default slot"));

	return GameProgressionSlots[0];
}

FGameProgressionSlot& UPDWPersistentUser::GetLastProgressionSlot()
{
	FDateTime LastTimeSaved = FDateTime::MinValue();
	int SlotIndex = 0;
	int SlotToSave = 0;

	for (FGameProgressionSlot& currentSlot : GameProgressionSlots)
	{
		if (currentSlot.SlotMetaData.LastSaved > LastTimeSaved)
		{
			SlotToSave = SlotIndex;
			LastTimeSaved = currentSlot.SlotMetaData.LastSaved;
		}
		SlotIndex++;
	}

	return GameProgressionSlots[SlotToSave];
}

bool UPDWPersistentUser::GetActivityCompletedOnce() const
{
	return bActivityCompleted;
}

void UPDWPersistentUser::SetActivityCompletedOnce(const bool bInCompleted)
{
	bActivityCompleted = bInCompleted;
}

void UPDWPersistentUser::RemoveProgressionSlotByIndex(int32 InIndex, UObject* WorldContext)
{
	if (InIndex >= 0 && InIndex < GAMEPROGRESSION_SLOTS_NUM)
	{
		FGameProgressionSlot NewEmptySlot;
		InitializeNewSlot(NewEmptySlot, WorldContext);
		GameProgressionSlots.RemoveAt(InIndex);
		GameProgressionSlots.EmplaceAt(InIndex, NewEmptySlot);
		CurrentUsedSlotIndex = CurrentUsedSlotIndex == InIndex ? -1 : CurrentUsedSlotIndex;
	}
}

bool UPDWPersistentUser::GetFirstBoot(UObject* WorldContextObject)
{
	return GetCurrentUseSlotIndex() < 0; //GetLastUsedSlotID?
}

void UPDWPersistentUser::InitializeNewSlot(FGameProgressionSlot& InSlot, UObject* WorldContext)
{
	//UPDWDataFunctionLibrary::SetPlayerAreaId(WorldContext, FGameplayTag::RequestGameplayTag("Map.DinoPlains"));
	FGameProgressionSlot& SavedData = InSlot;
	SavedData.PlayerData.LastVisitedAreaId = FGameplayTag::RequestGameplayTag("Map.DinoPlains");
	SavedData.PlayerData.VisitedAreaIds.AddUnique(FGameplayTag::RequestGameplayTag("Map.DinoPlains"));
	UE_LOG(LogTemp, Warning, TEXT("PDWPersistenUser - Initialize new save slot"));
}

void UPDWPersistentUser::InitSaves(UNebulaFlowGameInstance* InInstance)
{
	for (int i = 0; i < GAMEPROGRESSION_SLOTS_NUM; i++)
	{
		FGameProgressionSlot NewSlot;
		NewSlot.SlotMetaData.SlotIndex = -1;

		InitializeNewSlot(NewSlot, InInstance);
		GameProgressionSlots.Add(NewSlot);
	}

	// reset all settings
	GameOptions.Empty();

}

bool UPDWPersistentUser::HasSaveGames()
{
	for (int32 i = 0; i < GameProgressionSlots.Num(); ++i)
	{
		if (GameProgressionSlots[i].SlotMetaData.SlotIndex != -1)
		{
			return true;
		}
	}

	return false;
}

void UPDWPersistentUser::OnBeforeAutoSave()
{
	bool bWasSuccessful;
	FGameProgressionSlot& ProgressionSlot = GetCurrentUsedProgressionSlot(bWasSuccessful);
	if (bWasSuccessful)
	{
		ProgressionSlot.SetLastSaved(FDateTime::UtcNow());
	}
	FNebulaFlowCoreDelegates::OnBeforSave.Broadcast();
}

void UPDWPersistentUser::OnPostLoadFromSlot(int32 SlotID)
{
	if(SlotID < GAMEPROGRESSION_SLOTS_NUM)
	{ 
		CurrentUsedSlotIndex = SlotID;
	}
	FNebulaFlowCoreDelegates::OnPostLoad.Broadcast();
}

void UPDWPersistentUser::OnProfilePostLoad()
{
	CurrentUsedSlotIndex = FMath::Max(0,CurrentUsedSlotIndex);
	OnPostLoadFromSlot(CurrentUsedSlotIndex);
}
