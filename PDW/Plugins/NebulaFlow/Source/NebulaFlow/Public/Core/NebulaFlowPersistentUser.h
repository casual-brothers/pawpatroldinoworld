
#pragma once

#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "NebulaFlowGameInstance.h"
#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "Managers/NebulaFlowActivitiesManager.h"
#include "NebulaFlowPersistentUser.generated.h"

class UNebulaFlowGameInstance;

UCLASS()
class NEBULAFLOW_API UNebulaFlowPersistentUser : public USaveGame
{

	GENERATED_UCLASS_BODY()

public:

	/** Loads user persistence data if it exists, creates an empty record otherwise. */
	static UNebulaFlowPersistentUser* LoadPersistentUser(UNebulaFlowGameInstance* GInstance, FString SlotName, FString UserName, const int32 UserIndex, UClass* PersistentUserClass, bool& bNewPersistentUser)
	{
		UNebulaFlowPersistentUser* Result = nullptr;
		bNewPersistentUser = false;

		// first set of player signins can happen before the UWorld exists, which means no OSS, which means no user names, which means no slotnames.
		// Persistent users aren't valid in this state.
		if (SlotName.Len() > 0 && UserName.Len() > 0)
		{
			Result = Cast<UNebulaFlowPersistentUser>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
			if (Result == NULL)
			{
				// if failed to load, create a new one
				Result = Cast<UNebulaFlowPersistentUser>(UGameplayStatics::CreateSaveGameObject(PersistentUserClass));
				Result->GameInstanceRef = GInstance;
				Result->SaveGameSavedVersion = Result->SaveGameCurrentVersion;
				Result->InitSaves(GInstance);
				Result->SetToDefaults();
				bNewPersistentUser = true;
			}

			else if (Result->SaveGameCurrentVersion > Result->SaveGameSavedVersion)
			{
				UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
				return UNebulaFlowPersistentUser::LoadPersistentUser(GInstance, SlotName, UserName, UserIndex, PersistentUserClass, bNewPersistentUser);
			}

			check(Result != NULL);

			Result->GameInstanceRef = GInstance;
			Result->SlotName = SlotName;
			Result->UserName = UserName;
			Result->UserIndex = UserIndex;
		}

		return Result;
	}


	UPROPERTY(SaveGame)
	int32 SaveGameSavedVersion = 1;

	UPROPERTY(SaveGame)
	TArray<FRemappableAction> SavedRemappableActions{};

	UPROPERTY(SaveGame)
	TArray<FName> UnlockedAchievement;

	/** Saves data if anything has changed. */
	void SaveIfDirty(bool& OutResult, bool bAsyncSave = false);

	int32 GetUserIndex() const { return UserIndex; };

	FORCEINLINE FString GetSlotName() const
	{
		return SlotName;
	}
	FORCEINLINE FString GetUserName() const
	{
		return UserName;
	}

	bool IsSaving() const { return SaveTask != nullptr && !SaveTask->IsComplete(); }

	bool IsDirty() const { return bIsDirty; }

	void SetDirty() { bIsDirty = true; }

	void ClearDirtyFlag() { bIsDirty = false; }

	void WaitForSaveComplete() const { if (IsSaving()) FTaskGraphInterface::Get().WaitUntilTaskCompletes(SaveTask); }

	//		PS5 Activities START
	virtual TMap<FString, FActivityData>* GetActivitiesData();

	virtual TArray<FString>* GetPendingMatchesIDs();
	//		PS5 Activities END

protected:

	/** Internal.  True if data is changed but hasn't been saved. */
	bool bIsDirty;

	virtual void SetToDefaults();

	/** Triggers a save of this data. */
	void SavePersistentUser(bool& OutResult, bool bAsyncSave = false);

	virtual void InitSaves(UNebulaFlowGameInstance* GInstance);

	UPROPERTY()
	UNebulaFlowGameInstance* GameInstanceRef = nullptr;

	int32 SaveGameCurrentVersion = 1;

private:

	/** The string identifier used to save/load this persistent user. */
	FString SlotName;
	FString UserName;
	int32 UserIndex;

	FGraphEventRef SaveTask = nullptr;

	//PS5 ActivitiesData

	UPROPERTY(SaveGame)		//Data used to sync user activities progression 
		TMap<FString, FActivityData> ActivitiesData{};

	UPROPERTY(SaveGame)		//Data used to sync user activities progression 
		TArray<FString> PendingMathcesIds{};
};