#pragma once

#include "FlowSave.h"
#include "GameplayTagContainer.h"
#include "GameOptionsEnums.h"
#include "PDWGameplayStructures.h"
#include "Managers/NebulaFlowActivitiesManager.h"
#include "PDWSaveStructures.generated.h"


USTRUCT(BlueprintType)
struct PDW_API FDinoPenSaveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	bool IsActive = false;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FGameplayTag CurrentNeed = FGameplayTag();

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FGameplayTag CurrentFakeNeed = FGameplayTag();

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	int32 FoodInStation = 0;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	float WaterInStation = 0;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FGameplayTag DinoCustomization = {};
};

USTRUCT(BlueprintType)
struct PDW_API FInteractableData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FGameplayTagContainer IdentityTags = {};

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FGameplayTag State = {};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWMinigameInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	FGameplayTagContainer IdentityTags = {};

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	FGameplayTag Area = {};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MinigameInfo")
	FText Name = {};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MinigameInfo")
	FGameplayTag AllowedPup = {};

    bool operator==(const FPDWMinigameInfo& Other) const
    {
        return IdentityTags.HasAllExact(Other.IdentityTags);
    }
};

USTRUCT(BlueprintType)
struct PDW_API FWorldSaveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, bool> Facts;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TArray<int32> CollectedTreats;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FGameplayTagContainer ChestOpened;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, FDinoPenSaveData> DinoPensData;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<TObjectPtr<UDataLayerAsset>, bool> DLData = {};

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<int32,FInteractableData> QuestInteractableStates = {};

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TArray<int32> UsedInteractables = {};

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TArray<FQuestData> CompletedQuests = {};
	
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TArray<FPDWMinigameInfo> CompletedMinigames = {};
};


USTRUCT(BlueprintType)
struct PDW_API FFlowData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Flow")
	TArray<FFlowComponentSaveData> FlowComponents;

	UPROPERTY(VisibleAnywhere, Category = "Flow")
	TArray<FFlowAssetSaveData> FlowInstances;
};

USTRUCT(BlueprintType)
struct PDW_API FCustomizationData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, FGameplayTag> PupCustomizations;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, FGameplayTag> VehicleCustomizations;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, FGameplayTag> GuestPupCustomizations;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, FGameplayTag> GuestVehicleCustomizations;
};

USTRUCT(BlueprintType)
struct PDW_API FPlayerData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	FGameplayTag SelectedPup {};

	UPROPERTY(BlueprintReadWrite)
	bool IsPupOnVechicle = false;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	TMap<FGameplayTag, int32> Inventory;

	UPROPERTY(SaveGame, VisibleAnywhere, Category = "SaveData")
	FCustomizationData CustomizationData;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	int32 ExpPoints = 0;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	FTransform PlayerPosition = FTransform();

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	FGameplayTag LastVisitedAreaId{ FGameplayTag::EmptyTag };

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	TArray<FGameplayTag> VisitedAreaIds{};
	
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	FText CurrentQuestDescription{ FText::GetEmpty() };

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	TSet<FName> DisabledInputActions {};

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	FGameplayTagContainer AllowedPups {};
};

USTRUCT(BlueprintType)
struct PDW_API FAchievementsData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	int32 PupTreatsCollected = 0;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	int32 DinoNeedsSolved = 0;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	int32 OpenedChests = 0;
};

USTRUCT(BlueprintType)
struct PDW_API FGameProgressionSlotMetaData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	int32 SlotIndex{ -1 };
	
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	FDateTime LastSaved{ 0 };
	
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	float TotalElapsed{ 0.0f };

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "SaveData")
	bool IsNewGame = true;

};

USTRUCT(BlueprintType)
struct PDW_API FGameProgressionSlot
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	FGameProgressionSlotMetaData SlotMetaData{};

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	FWorldSaveData WorldData{};

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	FFlowData FlowData{};

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	FPlayerData PlayerData{};
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	FAchievementsData AchievementsData{};

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")		//Data used to sync user activities progression 
	TMap<FString, FActivityData> ActivitiesData{};

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "SaveData")
	bool isFirstStartedActivity = false;

	void SetLastSaved(FDateTime Value)
	{
		if (SlotMetaData.LastSaved > 0)
		{
			SlotMetaData.TotalElapsed += FTimespan(Value.GetTicks() - SlotMetaData.LastSaved.GetTicks()).GetTotalSeconds();
		}

		SlotMetaData.LastSaved = Value;
	}

};
