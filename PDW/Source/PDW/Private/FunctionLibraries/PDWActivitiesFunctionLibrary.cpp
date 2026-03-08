// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PDWActivitiesFunctionLibrary.h"
#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "PDWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/PDWActivitiesManager.h"
#include "Data/PDWLocalPlayer.h"

void UPDWActivitiesFunctionLibrary::StartMasterActivity(UObject* WorldContextObject)
{
	UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	ULocalPlayer* Player = GameInstance->GetLocalPlayerByIndex(0);
	UPDWActivitiesManager* Mgr = Cast<UPDWActivitiesManager>(UNebulaFlowActivitiesManager::GetFlowActivitiesManager(WorldContextObject));
	if (Mgr)
	{
		TMap<FString, FActivityData>* PlayerData =Mgr->GetPlayerActivitiesData(Player);
		if (PlayerData)
		{
			if (!PlayerData->Contains(Mgr->MasterActivityID))	// We have Clean Data, in first Initialization Main Acticvity and First Task will be available
			{
				//UE_LOG(LogTemp, Log, TEXT("PlayerData without %s"), *Mgr->MasterActivityId);

				Mgr->SetActivityAvailability(Mgr->MasterActivityID, true, Player);
				Mgr->SetActivityAvailability(Mgr->FirstTaskId, true, Player);
				SetActivityAlreadyStartedOnTheSlot(WorldContextObject,true);
				//ENOUGH? NO NEED TO SET UP OTHER ACTIVITIES, BY DEFAULT NOT AVAILABLE ALREADY.
			}
			else
			{
				if (PlayerData->Find(Mgr->MasterActivityID)->ActivityState == EActivityState::ENotStarted)
				{
					Mgr->SetActivityAvailability(Mgr->MasterActivityID, true, Player);
					Mgr->SetActivityAvailability(Mgr->FirstTaskId, true, Player);
					SetActivityAlreadyStartedOnTheSlot(WorldContextObject, true);
				}
				else if (PlayerData->Find(Mgr->MasterActivityID)->ActivityState == EActivityState::EStarted)
				{
					Mgr->SetActivityAvailability(Mgr->MasterActivityID, true, Player);
				}
			}
		}
	}
}

bool UPDWActivitiesFunctionLibrary::GetCompletedOnce(UObject* WorldContextObject)
{
	UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
	if (CurrentUser)
	{
		return CurrentUser->GetActivityCompletedOnce();
	}
	return false;
}

bool UPDWActivitiesFunctionLibrary::SetCompletedOnce(UObject* WorldContextObject, bool bInOption)
{
	UPDWPersistentUser* User = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
	if (User)
	{
		User->SetActivityCompletedOnce(bInOption);
		return true;
	}
	return false;
}

bool UPDWActivitiesFunctionLibrary::GetFirstBoot(UObject* WorldContextObject)
{
	UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
	if (CurrentUser)
	{
		return CurrentUser->GetCurrentUseSlotIndex() < 0; //GetLastUsedSlotID?
	}
	return true;
}

bool UPDWActivitiesFunctionLibrary::GetIsNewGame(UObject* WorldContextObject)
{
	UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
	if (CurrentUser)
	{	
		FGameProgressionSlot& Slot = CurrentUser->GetLastProgressionSlot();
		return CurrentUser->GetLastProgressionSlot().SlotMetaData.IsNewGame;
	}
	return true;
}

void UPDWActivitiesFunctionLibrary::SetActivityAlreadyStartedOnTheSlot(UObject* WorldContextObject, bool inOption)
{
	UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
	if (CurrentUser)
	{
		FGameProgressionSlot& currentSlot = CurrentUser->GetLastProgressionSlot();
		currentSlot.isFirstStartedActivity = inOption;
	}
}

void UPDWActivitiesFunctionLibrary::ForceResetAllActivities(UObject* WorldContextObject,const bool bInNewGame)
{
	UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	ULocalPlayer* Player = Cast<UPDWLocalPlayer>(GameInstance->GetLocalPlayerByIndex(0));
	UPDWActivitiesManager* Mgr = Cast<UPDWActivitiesManager>(UNebulaFlowActivitiesManager::GetFlowActivitiesManager(WorldContextObject));
	if (Mgr)
	{
		Mgr->SetNewGame(bInNewGame);
		Mgr->InitializeLocalPlayerActivities(Player);
	}
}