// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Interfaces/OnlineGameActivityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineError.h"
#include "Engine/DataTable.h"
#include "Interfaces/OnlineGameMatchesInterface.h"
#include "NebulaFlowActivitiesManager.generated.h"

class ULocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerActivities, Log, All);

UENUM(BlueprintType)
enum class EActivityType : uint8
{
	EActivity,
	ETask,
	ESubtask
};

UENUM(BlueprintType)
enum class EActivityState : uint8
{
	ENotStarted,
	EStarted,
	ECompleted
};

UENUM(BlueprintType)
enum class EActivityAvailability : uint8
{
	ENotAvailable,
	EAvailable
};


USTRUCT(BlueprintType)
struct NEBULAFLOW_API FActivityIdData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FString OwnerActivityId {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FString ActivityId {};

};

USTRUCT(BlueprintType)
struct NEBULAFLOW_API FActivityData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(SaveGame)
		FActivityIdData ActivityIdData;

	UPROPERTY(SaveGame)
		EActivityState ActivityState = EActivityState::ENotStarted;

	UPROPERTY(SaveGame)
		EActivityAvailability ActivityAvailability = EActivityAvailability::ENotAvailable;

};


USTRUCT(BlueprintType)
struct NEBULAFLOW_API FActivityTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Activity")
		FActivityIdData ActivityIdData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Activity")
		EActivityType ActivityType {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Activity")
		int32 ActivityIndex {};

};



UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowActivitiesManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Activities", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowActivitiesManager* GetFlowActivitiesManager(const UObject* WorldContextObject);

	virtual void Init(ULocalPlayer* InLocalPlayer);

	//ACTIVITY INTERFACE

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Activities")
		bool IsInitialized() { return bHasBeenInitialized; }

	/*Currently only Ps5 supports Activities*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Activities")
		static bool IsActivitiesAvailableOnCurrentPlatform();

	/* UDS Activity management wrapper functions*/

	UFUNCTION(BlueprintCallable, Category = "Activities")
		virtual void SetActivityAvailability(const FString& ActivityId, bool bEnabled, ULocalPlayer* LocalPlayer = nullptr, bool bForceEvenIfNotInitialized = false);
	UFUNCTION(BlueprintCallable, Category = "Activities")
		virtual void StartActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Activities")
		virtual void CompleteActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Activities")
		virtual  void FailActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Activities")
		virtual void AbandonActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Activities")
		virtual void AbortPendingOnlineMatches(ULocalPlayer* LocalPlayer, FString CurrentMatch = FString(""));

	virtual void AddPendingOnlineMatch(FString MatchId, ULocalPlayer* LocalPlayer);

	virtual void RemovePendingOnlineMatch(FString MatchId, ULocalPlayer* LocalPlayer);

	virtual void ResumeActivity(const FString& ActivityId, const  TArray<FString>& TaskIDsToComplete, const TArray<FString>& TaskIDsInProgress, ULocalPlayer* LocalPlayer = nullptr);

	void ResetAllActivities(ULocalPlayer* LocalPlayer = nullptr);

	const ULocalPlayer* GetCurrentactivityLocalPlayer() { return CurrentLocalPlayer; }

protected:


	//Activity operation completed callbacks
	static void OnActivityResetCompleted(const FUniqueNetId& LocalUserId, const FOnlineError& Status) {}
	static void OnActivityAvailiabilityCompleted(const FUniqueNetId& LocalUserId, const FOnlineError& Status);
	static void OnStartActivityCompleted(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineError& Status);
	static void OnEndActivityCompleted(const FUniqueNetId& LocalUserId, const FString& ActivityId, const EOnlineActivityOutcome& Outcome, const FOnlineError& Status);
	static void OnResumeActivityCompleted(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineError& Status);
	static void OnResetActivityCompleted(const FUniqueNetId& LocalUserId, const FOnlineError& Status);

	void OnMatchAbortedComplete(const FUniqueNetId& LocalUserId, const EUpdateGameMatchStatus& Status, const FOnlineError& Result, FString MatchToAbort);

	//in this table Activities, Tasks and subtasks data must be specified
	UPROPERTY(EditDefaultsOnly, Category = "Activities")
		UDataTable* ActivityTable;

	IOnlineGameActivityPtr GetActivityInterface(ULocalPlayer* LocalPlayer, FUniqueNetIdRepl& OutUniqueId);

	virtual void EndActivity(const FString& ActivityId, EOnlineActivityOutcome Outcome, ULocalPlayer* LocalPlayer = nullptr);

	virtual void InitializeLocalPlayerActivities(ULocalPlayer* LocalPlayer);

	virtual TMap<FString, FActivityData>* GetPlayerActivitiesData(ULocalPlayer* LocalPlayer = nullptr);

	FActivityTableRow*  GetActivityTableRow(const FString& ActivityId);

private:

	UPROPERTY()
		ULocalPlayer* CurrentLocalPlayer;

	bool bHasBeenInitialized = false;

	void SyncActivitiesStateGameData(FString ActivityId, EActivityState NewState);
	void SyncActivitiesAvailabilityGameData(FString ActivityId, EActivityAvailability NewAvailability);
	void SyncPendingMatches(ULocalPlayer* LocalPlayer);

	void InitCachedPendingMatches(ULocalPlayer* LocalPlayer);

	TArray<FString> CachedPendingMatches;

	static const FString ERROR_CONNECTION_TIMEDOUT;


};
