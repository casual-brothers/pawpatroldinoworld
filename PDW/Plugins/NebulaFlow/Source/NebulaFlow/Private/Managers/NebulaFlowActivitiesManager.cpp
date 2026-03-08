#include "Managers/NebulaFlowActivitiesManager.h"
#include "Interfaces/OnlineGameActivityInterface.h"
#include "Interfaces/OnlineEventsInterface.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "OnlineSubsystem.h"
#include "Core/NebulaFlowPersistentUser.h"
DEFINE_LOG_CATEGORY(LogPlayerActivities);

const FString UNebulaFlowActivitiesManager::ERROR_CONNECTION_TIMEDOUT = FString("0x804101e2");

UNebulaFlowActivitiesManager::UNebulaFlowActivitiesManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UNebulaFlowActivitiesManager::SetActivityAvailability(const FString& ActivityId, bool bEnabled, ULocalPlayer* LocalPlayer /*= nullptr*/, bool bForceEvenIfNotInitialized /*= false*/)
{
	if (!IsActivitiesAvailableOnCurrentPlatform())
	{
		return;
	}

	if (!bHasBeenInitialized && !bForceEvenIfNotInitialized)
	{
		return;
	}

	FUniqueNetIdRepl UniqueId;
	const IOnlineGameActivityPtr ActivityInterface = GetActivityInterface(LocalPlayer, UniqueId);
	FActivityTableRow* ActivityRowData = GetActivityTableRow(ActivityId);

	if (!ActivityInterface.IsValid() || !ActivityRowData)
	{
		return;
	}

	UE_LOG(LogPlayerActivities, Log, TEXT("SetActivityAvailability [%s][%s][%s]"), *ActivityId, *UniqueId.ToString(), bEnabled ? TEXT("Available") : TEXT("Unavailable"));

	SyncActivitiesAvailabilityGameData(ActivityId, bEnabled ? EActivityAvailability::EAvailable : EActivityAvailability::ENotAvailable);

	FOnSetActivityAvailabilityComplete ActivityAvailableDelegate = FOnSetActivityAvailabilityComplete::CreateStatic(&UNebulaFlowActivitiesManager::OnActivityAvailiabilityCompleted);
	ActivityInterface->SetActivityAvailability(*UniqueId, ActivityId, bEnabled, ActivityAvailableDelegate);
}

void UNebulaFlowActivitiesManager::StartActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	if (!IsActivitiesAvailableOnCurrentPlatform() || !bHasBeenInitialized)
	{
		return;
	}

	FUniqueNetIdRepl UniqueId;
	const IOnlineGameActivityPtr ActivityInterface = GetActivityInterface(LocalPlayer, UniqueId);
	FActivityTableRow* ActivityRowData = GetActivityTableRow(ActivityId);

	if (!ActivityInterface.IsValid() || !ActivityRowData)
	{
		return;
	}

	UE_LOG(LogPlayerActivities, Log, TEXT("StartActivity [%s][%s]"), *ActivityId, *UniqueId.ToString());

	SyncActivitiesStateGameData(ActivityId, EActivityState::EStarted);

	if (ActivityRowData && ActivityRowData->ActivityType == EActivityType::ETask)
	{
		SyncActivitiesStateGameData(ActivityRowData->ActivityIdData.OwnerActivityId, EActivityState::EStarted);
	}

	FOnlineEventParms Params;
	FOnStartActivityComplete StartActivityDelegate = FOnStartActivityComplete::CreateStatic(&UNebulaFlowActivitiesManager::OnStartActivityCompleted);
	ActivityInterface->StartActivity(*UniqueId, ActivityId, Params, StartActivityDelegate);
}

void UNebulaFlowActivitiesManager::CompleteActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	EndActivity(ActivityId, EOnlineActivityOutcome::Completed, LocalPlayer);
}

void UNebulaFlowActivitiesManager::FailActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	EndActivity(ActivityId, EOnlineActivityOutcome::Failed, LocalPlayer);
}

void UNebulaFlowActivitiesManager::AbandonActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	EndActivity(ActivityId, EOnlineActivityOutcome::Cancelled, LocalPlayer);
}

void UNebulaFlowActivitiesManager::AbortPendingOnlineMatches(ULocalPlayer* LocalPlayer, FString CurrentMatch)
{
	IOnlineGameMatchesPtr Matches = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem()->GetGameMatchesInterface();
	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer != nullptr ? LocalPlayer : CurrentLocalPlayer);

	if (Matches.IsValid() && CurrentPlayer)
	{
		SyncPendingMatches(LocalPlayer);
		if (CachedPendingMatches.Num() > 0 || CurrentMatch != FString(""))
		{
			FString MatchId = CurrentMatch == FString("") ? CachedPendingMatches.Last() : CurrentMatch;
			FOnGameMatchStatusUpdateComplete MatchStatusUpdateCompleteDelegate = FOnGameMatchStatusUpdateComplete::CreateLambda(
				[this, MatchId](const FUniqueNetId& LambdaLocalUserId, const EUpdateGameMatchStatus& LambdaStatus, const FOnlineError& LambdaResult)
			{
				OnMatchAbortedComplete(LambdaLocalUserId, LambdaStatus, LambdaResult, MatchId);
			});
			Matches->UpdateGameMatchStatus(*CurrentPlayer->GetPreferredUniqueNetId(), MatchId, EUpdateGameMatchStatus::Aborted, MatchStatusUpdateCompleteDelegate);
		}
	}
}


void UNebulaFlowActivitiesManager::AddPendingOnlineMatch(FString MatchId, ULocalPlayer* LocalPlayer)
{
	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer != nullptr ? LocalPlayer : CurrentLocalPlayer);
	if (CurrentPlayer)
	{
		CachedPendingMatches.AddUnique(MatchId);
		SyncPendingMatches(LocalPlayer);
	}
}

void UNebulaFlowActivitiesManager::RemovePendingOnlineMatch(FString MatchId, ULocalPlayer* LocalPlayer)
{
	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer != nullptr ? LocalPlayer : CurrentLocalPlayer);
	if (CurrentPlayer)
	{
		CachedPendingMatches.Remove(MatchId);
		SyncPendingMatches(LocalPlayer);
	}
}

void UNebulaFlowActivitiesManager::EndActivity(const FString& ActivityId, EOnlineActivityOutcome Outcome, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	if (!IsActivitiesAvailableOnCurrentPlatform() || !bHasBeenInitialized)
	{
		return;
	}

	FUniqueNetIdRepl UniqueId;
	const IOnlineGameActivityPtr ActivityInterface = GetActivityInterface(LocalPlayer, UniqueId);
	FActivityTableRow* ActivityRowData = GetActivityTableRow(ActivityId);

	if (!ActivityInterface.IsValid() || !ActivityRowData)
	{
		return;
	}

	UE_LOG(LogPlayerActivities, Log, TEXT("EndActivity [%s][%s][%d]"), *ActivityId, *UniqueId.ToString(), int(Outcome));

	if (Outcome == EOnlineActivityOutcome::Completed)
	{
		SyncActivitiesStateGameData(ActivityId, EActivityState::ECompleted);
		SyncActivitiesAvailabilityGameData(ActivityId, EActivityAvailability::ENotAvailable);
	}
	else
	{
		SyncActivitiesStateGameData(ActivityId, EActivityState::ENotStarted);
	}
	FOnlineEventParms Params;
	FOnEndActivityComplete EndActivityDelegate = FOnEndActivityComplete::CreateStatic(&UNebulaFlowActivitiesManager::OnEndActivityCompleted);
	ActivityInterface->EndActivity(*UniqueId, ActivityId, Outcome, Params, EndActivityDelegate);
}

void UNebulaFlowActivitiesManager::ResumeActivity(const FString& ActivityId, const  TArray<FString>& TaskIDsToComplete, const TArray<FString>& TaskIDsInProgress, ULocalPlayer* LocalPlayer /*= nullptr*/)
{
	if (!IsActivitiesAvailableOnCurrentPlatform() || !bHasBeenInitialized)
	{
		return;
	}

	FUniqueNetIdRepl UniqueId;
	const IOnlineGameActivityPtr ActivityInterface = GetActivityInterface(LocalPlayer, UniqueId);
	FActivityTableRow* ActivityRowData = GetActivityTableRow(ActivityId);

	if (!ActivityInterface.IsValid() || !ActivityRowData)
	{
		return;
	}

	UE_LOG(LogPlayerActivities, Log, TEXT("ResumeActivity [%s][%s]"), *ActivityId, *UniqueId.ToString());

	FOnlineEventParms Params;
	FOnResumeActivityComplete ResumeActivityDelegate = FOnResumeActivityComplete::CreateStatic(&UNebulaFlowActivitiesManager::OnResumeActivityCompleted);
	FOnlineActivityTasksToReset TasksToReset;
	TasksToReset.CompletedTasks = TaskIDsToComplete;
	TasksToReset.InProgressTasks = TaskIDsInProgress;
	ActivityInterface->ResumeActivity(*UniqueId, ActivityId, TasksToReset, ResumeActivityDelegate);
}

void UNebulaFlowActivitiesManager::ResetAllActivities(ULocalPlayer* LocalPlayer /*= nullptr*/)
{

	FUniqueNetIdRepl UniqueId;
	const IOnlineGameActivityPtr ActivityInterface = GetActivityInterface(LocalPlayer, UniqueId);

	if (!ActivityInterface.IsValid() || !IsActivitiesAvailableOnCurrentPlatform())
	{
		return;
	}
	FOnResetAllActiveActivitiesComplete ResetActivityDelegate = FOnResetAllActiveActivitiesComplete::CreateStatic(&UNebulaFlowActivitiesManager::OnActivityResetCompleted);
	ActivityInterface->ResetAllActiveActivities(*UniqueId, ResetActivityDelegate);
	UE_LOG(LogPlayerActivities, Log, TEXT("ResetAllActivities END"));

}

void UNebulaFlowActivitiesManager::OnActivityAvailiabilityCompleted(const FUniqueNetId & LocalUserId, const FOnlineError & Status)
{
	UE_LOG(LogPlayerActivities, Log, TEXT("OnActivityAvailiabilityCompleted: [%s][%s]"), *LocalUserId.ToString(), *Status.ToLogString());
}

void UNebulaFlowActivitiesManager::OnStartActivityCompleted(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineError& Status)
{
	UE_LOG(LogPlayerActivities, Log, TEXT("OnStartActivityCompleted: [%s][%s][%s]"), *ActivityId, *LocalUserId.ToString(), *Status.ToLogString());
}

void UNebulaFlowActivitiesManager::OnEndActivityCompleted(const FUniqueNetId& LocalUserId, const FString& ActivityId, const EOnlineActivityOutcome& Outcome, const FOnlineError& Status)
{
	UE_LOG(LogPlayerActivities, Log, TEXT("OnEndActivityCompleted: [%s][%s][%s][%d]"), *ActivityId, *LocalUserId.ToString(), *Status.ToLogString(), int(Outcome));
}

void UNebulaFlowActivitiesManager::OnResumeActivityCompleted(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineError& Status)
{
	UE_LOG(LogPlayerActivities, Log, TEXT("OnResumeActivityCompleted: [%s][%s][%s]"), *ActivityId, *LocalUserId.ToString(), *Status.ToLogString());
}



UNebulaFlowActivitiesManager* UNebulaFlowActivitiesManager::GetFlowActivitiesManager(const UObject* WorldContextObject)
{
	if(IsActivitiesAvailableOnCurrentPlatform())
	{
		UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
		if (GInstance)
		{
			if (!GInstance->FlowActivitiesManagerInstance && GInstance->ActivitiesManagerClass)
			{
				GInstance->FlowActivitiesManagerInstance = NewObject<UNebulaFlowActivitiesManager>(GInstance,GInstance->ActivitiesManagerClass);
			}
			return GInstance->FlowActivitiesManagerInstance;
		}		
	}
	return nullptr;
}

//INIT MUST BE CALLED AFTER PROFILE LOADING!

void UNebulaFlowActivitiesManager::Init(ULocalPlayer* InLocalPlayer)
{
	if (!bHasBeenInitialized && IsActivitiesAvailableOnCurrentPlatform())
	{
		bHasBeenInitialized = true;
		CurrentLocalPlayer = InLocalPlayer;
		InitializeLocalPlayerActivities(InLocalPlayer);
		InitCachedPendingMatches(InLocalPlayer);
		AbortPendingOnlineMatches(InLocalPlayer);
	}
}





bool UNebulaFlowActivitiesManager::IsActivitiesAvailableOnCurrentPlatform()
{
#if PLATFORM_PS5
	return true;
#endif
	return false;
}


void UNebulaFlowActivitiesManager::OnResetActivityCompleted(const FUniqueNetId& LocalUserId, const FOnlineError& Status)
{
	UE_LOG(LogPlayerActivities, Log, TEXT("OnResumeActivityCompleted: [%s][%s]"), *LocalUserId.ToString(), *Status.ToLogString());
}


void UNebulaFlowActivitiesManager::OnMatchAbortedComplete(const FUniqueNetId& LocalUserId, const EUpdateGameMatchStatus& Status, const FOnlineError& Result, FString MatchToAbort)
{

	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(CurrentLocalPlayer);
	if (CurrentPlayer)
	{
		if (CachedPendingMatches.Num() > 0 && (Result.WasSuccessful() || !Result.GetErrorCode().Contains(ERROR_CONNECTION_TIMEDOUT)))
		{
			CachedPendingMatches.Remove(MatchToAbort);
			SyncPendingMatches(CurrentPlayer);
			UE_LOG(LogPlayerActivities, Log, TEXT("Match Sussessfully aborted"));
			if (CachedPendingMatches.Num() > 0)
			{
				AbortPendingOnlineMatches(CurrentPlayer);
			}

		}
	}
	return;

}

IOnlineGameActivityPtr UNebulaFlowActivitiesManager::GetActivityInterface(ULocalPlayer* LocalPlayer, FUniqueNetIdRepl& OutUniqueId)
{
	if (!LocalPlayer && !CurrentLocalPlayer)
	{
		UE_LOG(LogPlayerActivities, Warning, TEXT("Local Player not Valid"));
		return nullptr;
	}

	OutUniqueId = LocalPlayer != nullptr ? LocalPlayer->GetPreferredUniqueNetId() : CurrentLocalPlayer->GetPreferredUniqueNetId();
	if (!OutUniqueId.IsValid())
	{
		UE_LOG(LogPlayerActivities, Warning, TEXT("UniqueId not Valid"));
		return nullptr;
	}
	IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
	if (!OnlineSub)
	{
		UE_LOG(LogPlayerActivities, Warning, TEXT("No online subsystem!"));
		return nullptr;
	}

	const IOnlineGameActivityPtr ActivityInterface = OnlineSub->GetGameActivityInterface();
	if (!ActivityInterface.IsValid())
	{
		UE_LOG(LogPlayerActivities, Warning, TEXT("No Activity Interface!"));
		return nullptr;
	}

	return ActivityInterface;
}

void UNebulaFlowActivitiesManager::InitializeLocalPlayerActivities(ULocalPlayer* LocalPlayer)
{
	//override in derived function (Game specific logics): override this function to specify activity initialization logics based on player saved datas
}

TMap<FString, FActivityData>* UNebulaFlowActivitiesManager::GetPlayerActivitiesData(ULocalPlayer* LocalPlayer)
{
	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer != nullptr ? LocalPlayer : CurrentLocalPlayer);

	if (CurrentPlayer)
	{
		UNebulaFlowPersistentUser* PersistentUser = CurrentPlayer->GetPersistentUser();
		if (PersistentUser)
		{
			return PersistentUser->GetActivitiesData();
		}
	}
	return nullptr;
}

void UNebulaFlowActivitiesManager::SyncActivitiesAvailabilityGameData(FString ActivityId, EActivityAvailability NewAvailability)
{
	TMap<FString, FActivityData>* CurrentActivityData = GetPlayerActivitiesData();
	if (CurrentActivityData)
	{
		if (CurrentActivityData->Contains(ActivityId))
		{
			CurrentActivityData->Find(ActivityId)->ActivityAvailability = NewAvailability;
		}
		else
		{
			FActivityData NewActivityData;
			NewActivityData.ActivityIdData.ActivityId = ActivityId;
			NewActivityData.ActivityAvailability = NewAvailability;
			CurrentActivityData->Add(ActivityId, NewActivityData);
		}
	}
}

void UNebulaFlowActivitiesManager::SyncPendingMatches(ULocalPlayer* LocalPlayer)
{
	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer != nullptr ? LocalPlayer : CurrentLocalPlayer);
	if (CurrentPlayer)
	{
		UNebulaFlowPersistentUser* PersistentUser = CurrentPlayer->GetPersistentUser();
		if (PersistentUser && PersistentUser->GetPendingMatchesIDs())
		{
			PersistentUser->GetPendingMatchesIDs()->Empty();
			for (FString currentMatchId : CachedPendingMatches)
			{
				PersistentUser->GetPendingMatchesIDs()->Add(currentMatchId);
			}
			PersistentUser->SetDirty();
		}
	}
}

void UNebulaFlowActivitiesManager::InitCachedPendingMatches(ULocalPlayer* LocalPlayer)
{
	UNebulaFlowLocalPlayer* CurrentPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer != nullptr ? LocalPlayer : CurrentLocalPlayer);
	if (CurrentPlayer)
	{
		UNebulaFlowPersistentUser* PersistentUser = CurrentPlayer->GetPersistentUser();
		if (PersistentUser)
		{
			TArray<FString>* SavedMatchesIds = PersistentUser->GetPendingMatchesIDs();
			CachedPendingMatches.Empty();
			if (PersistentUser && SavedMatchesIds && SavedMatchesIds->Num() > 0)
			{
				for (FString currentMatchId : *SavedMatchesIds)
				{
					CachedPendingMatches.Add(currentMatchId);
				}
			}
		}
	}
}


FActivityTableRow* UNebulaFlowActivitiesManager::GetActivityTableRow(const FString& ActivityId)
{
	if (ActivityTable)
	{
		TArray<FActivityTableRow*> Rows;
		ActivityTable->GetAllRows(FString("GENERAL"), Rows);
		if (Rows.Num() > 0)
		{
			FActivityTableRow* OutTableRow = *Rows.FindByPredicate([=, this](const FActivityTableRow* Row)
			{
				return Row->ActivityIdData.ActivityId == ActivityId;
			});
			if (OutTableRow == nullptr)
			{
				UE_LOG(LogPlayerActivities, Warning, TEXT("Activity [%s] Not found in Activities Data Table! "), *ActivityId);
			}
			return OutTableRow;
		}

	}
	UE_LOG(LogPlayerActivities, Warning, TEXT("No Activity Data Table Defined!"));
	return nullptr;
}

void UNebulaFlowActivitiesManager::SyncActivitiesStateGameData(FString ActivityId, EActivityState NewState)
{
	TMap<FString, FActivityData>* CurrentActivityData = GetPlayerActivitiesData();
	if (CurrentActivityData)
	{
		if (CurrentActivityData->Contains(ActivityId))
		{
			CurrentActivityData->Find(ActivityId)->ActivityState = NewState;
		}
		else
		{
			FActivityData NewActivityData;
			NewActivityData.ActivityIdData.ActivityId = ActivityId;
			NewActivityData.ActivityState = NewState;

			CurrentActivityData->Add(ActivityId, NewActivityData);
		}
	}
}
