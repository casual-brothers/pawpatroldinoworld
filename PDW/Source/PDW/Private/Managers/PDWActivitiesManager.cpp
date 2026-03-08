// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWActivitiesManager.h"
#include "FunctionLibraries/PDWActivitiesFunctionLibrary.h"
#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "PDWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Data/PDWLocalPlayer.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogPDWctivities);
void UPDWActivitiesManager::StartActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	
	TMap<FString, FActivityData>* PlayerData = GetPlayerActivitiesData();
	if (PlayerData)
	{
		if (PlayerData->Contains(ActivityId) && PlayerData->Find(ActivityId)->ActivityState != EActivityState::ENotStarted)	//Start activity only if it is not already started
		{
			return;
		}
	}

	if (!ActivityId.Equals(MasterActivityID, ESearchCase::IgnoreCase))
	{
		CurrentStartedTask = ActivityId;
	}

	UE_LOG(LogPDWctivities, Log, TEXT("LogPDWctivities::StartActivity activity: %s"), *ActivityId);
	Super::StartActivity(ActivityId, LocalPlayer);
}

void UPDWActivitiesManager::SetActivityAvailability(const FString& ActivityId, bool bEnabled, ULocalPlayer* LocalPlayer /*= nullptr*/, bool bForceEvenIfNotInitialized /*= false*/)
{
	FUniqueNetIdRepl UniqueId;
	const IOnlineGameActivityPtr ActivityInterface = GetActivityInterface(LocalPlayer, UniqueId);
	FOnSetActivityAvailabilityComplete ActivityAvailableDelegate = FOnSetActivityAvailabilityComplete::CreateStatic(&UNebulaFlowActivitiesManager::OnActivityAvailiabilityCompleted);
	Super::SetActivityAvailability(ActivityId, bEnabled, LocalPlayer, bForceEvenIfNotInitialized);
}

void UPDWActivitiesManager::SetNewGame(const bool bInNewGame)
{
	bIsNewGame = bInNewGame;
}


TMap<FString, FActivityData>* UPDWActivitiesManager::GetPlayerActivitiesData(ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	UPDWPersistentUser* User = UPDWDataFunctionLibrary::GetPersistentUser(GetWorld());
	if (User)
	{
		FGameProgressionSlot& currentSlot = User->GetLastProgressionSlot();
		return &currentSlot.ActivitiesData;
	}
	return nullptr;
}

void UPDWActivitiesManager::EndActivity(const FString& ActivityId, EOnlineActivityOutcome Outcome, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	TMap<FString, FActivityData>* PlayerData = GetPlayerActivitiesData(LocalPlayer);
	if (PlayerData)
	{
		if (PlayerData->Contains(ActivityId) && PlayerData->Find(ActivityId)->ActivityState == EActivityState::ECompleted)
		{
			return;
		}
	}
	
	if (!ActivityId.Equals(MasterActivityID, ESearchCase::IgnoreCase))
	{
		CurrentStartedTask = FString("");
	}
	Super::EndActivity(ActivityId, Outcome, LocalPlayer);

	if (Outcome == EOnlineActivityOutcome::Completed)
	{
		FActivityTableRow* TaskRow = GetActivityTableRow(ActivityId);
		if (TaskRow && TaskRow->ActivityType == EActivityType::ETask)
		{
			SetActivityAvailability(ActivityId, false, LocalPlayer);
			if (LastTaskId.Equals(TaskRow->ActivityIdData.ActivityId, ESearchCase::IgnoreCase))
			{
				EndActivity(TaskRow->ActivityIdData.OwnerActivityId, Outcome, LocalPlayer);
				SetActivityAvailability(TaskRow->ActivityIdData.OwnerActivityId, false, LocalPlayer);
				
				UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(this);
				if (CurrentUser && CurrentUser->GetCurrentUseSlotIndex() >= 0)	//GetLastUsedSlotID?
				{
					UPDWActivitiesFunctionLibrary::SetCompletedOnce(this,true);
				}
			}
			else
			{
				SetActivityAvailability(GetNextTaskId(TaskRow->ActivityIndex), true, LocalPlayer);
			}
		}
	}
	UE_LOG(LogPDWctivities, Log, TEXT("LogPDWctivities::EndActivity activity: %s"), *ActivityId);
}

void UPDWActivitiesManager::InitializeLocalPlayerActivities(ULocalPlayer* LocalPlayer)
{
	UE_LOG(LogPDWctivities, Log, TEXT("InitializeLocalPlayerActivities"));

	Super::InitializeLocalPlayerActivities(LocalPlayer);

	TMap<FString, FActivityData>* PlayerData = GetPlayerActivitiesData(LocalPlayer);

	ResetAllActivities(LocalPlayer);	//First Reset All activities status when Manager is initialized

	bool bActivityCompletedOnce = UPDWActivitiesFunctionLibrary::GetCompletedOnce(this);

	bool bIsFirstBootOfTheGame = UPDWActivitiesFunctionLibrary::GetFirstBoot(this);

	bool CheckNewGame = UPDWActivitiesFunctionLibrary::GetIsNewGame(this);
	
	if (bActivityCompletedOnce)
	{
		UE_LOG(LogPDWctivities, Log, TEXT("PlayerData contain completed activity: %s"), *MasterActivityID);

		SetActivityAvailability(MasterActivityID, false, LocalPlayer);
		return;
	}

	//FIRST START, NEVER PLAYED


	if (bIsFirstBootOfTheGame || CheckNewGame) //Setting everything at not available.
	{
		TArray<FActivityTableRow*> ActivityRows;
		ActivityTable->GetAllRows(FString("GENERAL"), ActivityRows);
		for (FActivityTableRow* currentRow : ActivityRows)
		{
			FString currentActivityId = currentRow->ActivityIdData.ActivityId;
			SetActivityAvailability(currentActivityId, false, LocalPlayer);
		}
		return;
	}

	if (PlayerData)
	{
			//REMEMBER EVERYTHING IS RESETTED
		if (PlayerData->Contains(MasterActivityID))
		{
			TArray<FActivityTableRow*> ActivityRows;
			ActivityTable->GetAllRows(FString("GENERAL"), ActivityRows);
			TArray<FString> CompletedTasks{};
			TArray<FString> InProgressTasks{};
			for (FActivityTableRow* currentRow : ActivityRows)
			{
				FString currentActivityId = currentRow->ActivityIdData.ActivityId;
				if (currentRow->ActivityType == EActivityType::ETask)
				{
					if (!PlayerData->Contains(currentActivityId))
					{
						continue;
					}
					SetActivityAvailability(currentActivityId, PlayerData->Find(currentActivityId)->ActivityAvailability == EActivityAvailability::EAvailable, LocalPlayer);
					if (PlayerData->Find(currentActivityId)->ActivityState == EActivityState::ECompleted)
					{
						CompletedTasks.Add(currentActivityId);
					}
					else
					{
						InProgressTasks.Add(currentActivityId);
					}
				}
			}
			if (PlayerData->Find(MasterActivityID)->ActivityState == EActivityState::EStarted)	// if main activity is started resume with correct progression
			{
				ResumeActivity(MasterActivityID, CompletedTasks, InProgressTasks, LocalPlayer);
			}
		}
	}
}

FString UPDWActivitiesManager::GetNextTaskId(int32 inCurrentTaskIndex)
{
	TArray<FActivityTableRow*> ActivityRows;
	ActivityTable->GetAllRows(FString("GENERAL"), ActivityRows);
	if (ActivityRows.Num())
	{
		FActivityTableRow* currentRow = *ActivityRows.FindByPredicate([=](const FActivityTableRow* current)
		{
			return current->ActivityIndex == inCurrentTaskIndex + 1;
		});

		return (currentRow != nullptr ? currentRow->ActivityIdData.ActivityId : FString(""));
	}
	return FString("");
}