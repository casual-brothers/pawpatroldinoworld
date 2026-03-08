#include "LeaderboardManagment/LeaderboardManagementSubsystem.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "NebulaFlow.h"

void ULeaderboardManagementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LeaderboardRequestCompleteDelegate = FOnLeaderboardReadCompleteDelegate::CreateUObject(this, &ThisClass::OnLeaderboardReadComplete);
}

void ULeaderboardManagementSubsystem::Deinitialize()
{
	LeaderboardRequestCompleteDelegate.Unbind();
}

bool ULeaderboardManagementSubsystem::RequestUpdateStat(const FLeadearboardWriteRequestParams& LeaderboardWriteRequestParams)
{
	return LeaderboardWrite(LeaderboardWriteRequestParams);
}

void ULeaderboardManagementSubsystem::RequestGetStat(const FLeaderboardReadRequestParams& LeaderboardWriteRequestParams)
{
	if (EnqueueReadRequest.Num() == 2)
	{
		EnqueueReadRequest[1] = LeaderboardWriteRequestParams;
	}
	else
	{
		EnqueueReadRequest.Add(LeaderboardWriteRequestParams);
	}
	if (EnqueueReadRequest.Num() == 1)
	{
		if (bCanRequest)
		{
			LeaderboardRead(LeaderboardWriteRequestParams);
		}
	}
}

bool ULeaderboardManagementSubsystem::LeaderboardWrite(const FLeadearboardWriteRequestParams& LeaderboardWriteRequestParams)
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player."));
		return false;
	}
	IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
	if (!OnlineSub)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
		return false;
	}
	IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
	if (!Identity)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid identity interface."));
		return false;
	}
	TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
	if (!UserId.IsValid())
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid user id for this controller."));
		return false;
	}
	APlayerState* PlayerState = UGameplayStatics::GetPlayerStateFromUniqueNetId(this, UserId);
	if (!PlayerState)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid Player State."));
		return false;
	}
	auto Leaderboards = OnlineSub->GetLeaderboardsInterface();
	if (!Leaderboards)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid Leaderboard object."));
		return false;
	}
	FOnlineLeaderboardWrite LeaderboardWriteObject;
	LeaderboardWriteObject.LeaderboardNames.Add(LeaderboardWriteRequestParams.LeaderboardName);
	LeaderboardWriteObject.RatedStat = LeaderboardWriteRequestParams.MainStat;
	LeaderboardWriteObject.DisplayFormat = LeaderboardWriteRequestParams.FormatType;
	LeaderboardWriteObject.SortMethod = LeaderboardWriteRequestParams.SortMethod;
	LeaderboardWriteObject.UpdateMethod = LeaderboardWriteRequestParams.UpdateMethod;
	for (const auto& [Statistics, Value] : LeaderboardWriteRequestParams.GetStats())
	{
		LeaderboardWriteObject.SetIntStat(Statistics, Value);
	}
	if (Leaderboards->WriteLeaderboards(PlayerState->SessionName, *UserId, LeaderboardWriteObject))
	{
		return Leaderboards->FlushLeaderboards(PlayerState->SessionName);
	}
	return false;
}

bool ULeaderboardManagementSubsystem::LeaderboardRead(const FLeaderboardReadRequestParams& LeaderboardReadRequestParams)
{
	bCanRequest = false;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player."));
		return false;
	}
	IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
	if (!OnlineSub)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
		return false;
	}
	IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
	if (!Identity)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid identity interface."));
		return false;
	}
	TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
	if (!UserId.IsValid())
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid user id for this controller."));
		return false;
	}
	auto Leaderboards = OnlineSub->GetLeaderboardsInterface();
	if (!Leaderboards)
	{ 
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid Leaderboard."));
		return false;
	}	 
	ReadObject = MakeShareable(new FOnlineLeaderboardRead);
	FOnlineLeaderboardReadRef ReadObjectRef = ReadObject.ToSharedRef();
	ReadObjectRef->SortedColumn = LeaderboardReadRequestParams.SortedColums;
	ReadObjectRef->LeaderboardName = LeaderboardReadRequestParams.LeaderboardName;
	for (const auto& [Column, ColumnType] : LeaderboardReadRequestParams.GetColumns())
	{
		ReadObjectRef->ColumnMetadata.Add({ Column, ColumnType });
	}
	if (auto* LeaderboardQueryPlayerParams = LeaderboardReadRequestParams.QueryParams.TryGet<FLeaderboardQueryPlayerParams>())
	{
		TArray<FUniqueNetIdRef> QueryPlayer;
		QueryPlayer.Add(UserId.ToSharedRef());
		LeaderboardReadCompleteDelegateHandle = Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(LeaderboardRequestCompleteDelegate);
		Leaderboards->ReadLeaderboards(QueryPlayer, ReadObjectRef);

	}
	else if (auto* LeaderboardQueryAroundRankParams = LeaderboardReadRequestParams.QueryParams.TryGet<FLeaderboardQueryAroundRankParams>())
	{	
		LeaderboardReadCompleteDelegateHandle = Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(LeaderboardRequestCompleteDelegate);
		Leaderboards->ReadLeaderboardsAroundRank(LeaderboardQueryAroundRankParams->Rank, LeaderboardQueryAroundRankParams->Range, ReadObjectRef);
	}
	else if (auto* LeaderboardQueryAroundUserParams = LeaderboardReadRequestParams.QueryParams.TryGet<FLeaderboardQueryAroundUserParams>())
	{
		LeaderboardReadCompleteDelegateHandle = Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(LeaderboardRequestCompleteDelegate);
		Leaderboards->ReadLeaderboardsAroundUser(UserId.ToSharedRef(), LeaderboardQueryAroundUserParams->Range, ReadObjectRef);
	}
	else if (auto* LeaderboardQueryTopAndAroundUserParams = LeaderboardReadRequestParams.QueryParams.TryGet<FLeaderboardQueryTopAndAroundUserParams>())
	{
		LeaderboardReadCompleteDelegateHandle = Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(LeaderboardRequestCompleteDelegate);
		Leaderboards->ReadLeaderboardsAroundRank(1, LeaderboardQueryTopAndAroundUserParams->Range, ReadObjectRef);
	}
	return true;
}

void ULeaderboardManagementSubsystem::AppendUnique(const TArray<FOnlineStatsRow>& iReadResults)
{
	for (const auto& ReadResult : iReadResults)
	{
		bool bCanAddResult = true;
		for (const auto& CurrentLeaderboardResult : LeaderboardReadResult)
		{
			if (CurrentLeaderboardResult.NickName == ReadResult.NickName)
			{
				bCanAddResult = false;
				break;
			}
		}
		if (bCanAddResult)
		{
			LeaderboardReadResult.Add(ReadResult);
		}
	}
}

void ULeaderboardManagementSubsystem::OnLeaderboardReadComplete(bool bWasSuccessful)
{
	UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("OnLeaderboardReadComplete bWasSuccessful: %d"), bWasSuccessful);
	if (bWasSuccessful)
	{
		IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
		if (!OnlineSub)
		{
			UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
			return;
		}
		auto Leaderboards = OnlineSub->GetLeaderboardsInterface();
		if (!Leaderboards)
		{
			UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid Leaderboard."));
			return;
		}
		Leaderboards->ClearOnLeaderboardReadCompleteDelegate_Handle(LeaderboardReadCompleteDelegateHandle);
		AppendUnique(ReadObject->Rows);
		//LeaderboardReadResult.Append(ReadObject->Rows);
		bool bIsOver = true;
		if (EnqueueReadRequest[0].QueryParams.IsType<FLeaderboardQueryTopAndAroundUserParams>())
		{
			bIsOver = false;
			EnqueueReadRequest[0].QueryParams.Set<FLeaderboardQueryPlayerParams>({});
			LeaderboardRead(EnqueueReadRequest[0]);
		}
		if (bIsOver)
		{
			EnqueueReadRequest.RemoveAt(0);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=, this]() {
				bCanRequest = true;
				if (EnqueueReadRequest.Num() != 0)
				{
					LeaderboardRead(EnqueueReadRequest[0]);
				}
				}, TimerDuration, false);
			OnLeaderboardReadCompleteDelegate.Broadcast(LeaderboardReadResult);
			LeaderboardReadResult.Empty();
		}
	}
	else
	{
		EnqueueReadRequest.RemoveAt(0);
	}
}

FLeaderboardReadRequestParams::FLeaderboardReadRequestParams()
{
	QueryParams.Set<FLeaderboardQueryPlayerParams>(FLeaderboardQueryPlayerParams{});
}

FReadResult::FReadResult(const FOnlineStatsRow& Other)
{
	NickName = Other.NickName;
	Rank = Other.Rank;
	for (const auto& Column : Other.Columns)
	{
		int32 Value = 0;
		Column.Value.GetValue(Value);
		Columns.Add(Column.Key, Value);
	}
}
