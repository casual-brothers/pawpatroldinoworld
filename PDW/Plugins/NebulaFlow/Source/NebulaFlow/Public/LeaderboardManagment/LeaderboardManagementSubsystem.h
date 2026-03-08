#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "LeaderboardManagementSubsystem.generated.h"


USTRUCT()
struct NEBULAFLOW_API FLeadearboardWriteRequestParams
{
	GENERATED_BODY()

public:

	FString LeaderboardName = TEXT("");
	FString MainStat = TEXT("");
	ELeaderboardFormat::Type FormatType = ELeaderboardFormat::Number; 
	ELeaderboardSort::Type SortMethod = ELeaderboardSort::Ascending; 
	ELeaderboardUpdateMethod::Type UpdateMethod = ELeaderboardUpdateMethod::KeepBest;
	void AddStat(const FString& Name, int32 Value) { Stats.Add(TPair<FString, int32>(Name, Value));	};
	const TArray<TPair<FString, int32>>& GetStats() const { return Stats; }

private:

	TArray<TPair<FString, int32>> Stats;
};

USTRUCT()
struct NEBULAFLOW_API FLeaderboardQueryPlayerParams
{
	GENERATED_BODY()
};

USTRUCT()
struct NEBULAFLOW_API FLeaderboardQueryAroundRankParams
{
	GENERATED_BODY()

public:

	int32 Rank = 1;
	int32 Range = 10;
};

USTRUCT()
struct NEBULAFLOW_API FLeaderboardQueryAroundUserParams
{
	GENERATED_BODY()

public:

	int32 Range = 10;
};

USTRUCT()
struct NEBULAFLOW_API FLeaderboardQueryTopAndAroundUserParams
{
	GENERATED_BODY()

public:

	int32 Range = 10;
};

USTRUCT()
struct NEBULAFLOW_API FLeaderboardReadRequestParams
{
	GENERATED_BODY()

public:

	FLeaderboardReadRequestParams();
	FString SortedColums;
	FString LeaderboardName;
	void AddColumn(const FString& Column, EOnlineKeyValuePairDataType::Type Type) { Columns.Add(TPair<FString, EOnlineKeyValuePairDataType::Type>(Column, Type)); };
	const TArray<TPair<FString, EOnlineKeyValuePairDataType::Type>>& GetColumns() const { return Columns; }
	TVariant<FLeaderboardQueryPlayerParams, FLeaderboardQueryAroundRankParams, FLeaderboardQueryAroundUserParams, FLeaderboardQueryTopAndAroundUserParams> QueryParams;

private:

	TArray<TPair<FString, EOnlineKeyValuePairDataType::Type>> Columns;

};

USTRUCT()
struct NEBULAFLOW_API FReadResult
{

GENERATED_BODY()

public:

	FReadResult(){};
	FReadResult(const FOnlineStatsRow& Other);


	/** Name of player in this row */
	FString NickName;
	/** Player's rank in this leaderboard */
	int32 Rank;
	/** All requested data on the leaderboard for this player */
	TMap<FString, int32> Columns;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderboardReadDelegate, const TArray<FReadResult>&, LeaderboardResults);

UCLASS()
class NEBULAFLOW_API ULeaderboardManagementSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	bool RequestUpdateStat(const FLeadearboardWriteRequestParams& LeaderboardWriteRequestParams);
	void RequestGetStat(const FLeaderboardReadRequestParams& LeaderboardWriteRequestParams);

	FOnLeaderboardReadDelegate OnLeaderboardReadCompleteDelegate;

protected:

	void OnLeaderboardReadComplete(bool bWasSuccessful);

	bool LeaderboardWrite(const FLeadearboardWriteRequestParams& LeaderboardWriteRequestParams);
	bool LeaderboardRead(const FLeaderboardReadRequestParams& LeaderboardReadRequestParams);

	void AppendUnique(const TArray<FOnlineStatsRow>& iReadResults);

private:

	FOnLeaderboardReadCompleteDelegate LeaderboardRequestCompleteDelegate;
	FDelegateHandle LeaderboardReadCompleteDelegateHandle;
	FOnlineLeaderboardReadPtr ReadObject;

	TArray<FLeaderboardReadRequestParams> EnqueueReadRequest;

	bool bCanRequest = true;

	FTimerHandle TimerHandle;
	float TimerDuration = .5f;

	TArray<FReadResult> LeaderboardReadResult;

};