#include "Core/NebulaFlowPersistentUser.h"
#include "Kismet/GameplayStatics.h"





//Async SaveGameTask

class FDispatchSaveGame
{
public:
	FDispatchSaveGame(USaveGame* SaveGameObject, FString SlotName, int32 UserIndex)
		: SaveGame(SaveGameObject)
		, Slot(SlotName)
		, UserIdx(UserIndex)
	{
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FDispatchSaveGame, STATGROUP_TaskGraphTasks);
	}

	ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::AnyThread;
	}

	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		/** Other tasks can depend on this task.. */
		return ESubsequentsMode::TrackSubsequents;
	}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		UGameplayStatics::SaveGameToSlot(SaveGame, Slot, UserIdx);
	}

private:
	USaveGame* SaveGame;
	FString Slot;
	int32 UserIdx;
};







UNebulaFlowPersistentUser::UNebulaFlowPersistentUser(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}



void UNebulaFlowPersistentUser::SaveIfDirty(bool& OutResult, bool bAsyncSave /*= false*/)
{
	if (bIsDirty)
	{
		SavePersistentUser(OutResult, bAsyncSave);
	}
}

TMap<FString, FActivityData>* UNebulaFlowPersistentUser::GetActivitiesData()
{
	return &ActivitiesData;
}

TArray<FString>* UNebulaFlowPersistentUser::GetPendingMatchesIDs()
{
	return &PendingMathcesIds;
}

void UNebulaFlowPersistentUser::SetToDefaults()
{
	bIsDirty=false;
}

void UNebulaFlowPersistentUser::SavePersistentUser(bool& OutResult,bool bAsyncSave /*= false*/)
{
	if (bAsyncSave)
	{
		OutResult = true;
		FGraphEventArray Prereq;
		SaveTask = TGraphTask<FDispatchSaveGame>::CreateTask(&Prereq, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(this, SlotName, UserIndex);
	}
	else
	{
		OutResult = UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
	}
	bIsDirty = false;
}

void UNebulaFlowPersistentUser::InitSaves(UNebulaFlowGameInstance* GInstance)
{
	//to Override
}
