// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/NebulaFlowPersistentUser.h"
#include "Data/PDWSaveStructures.h"
#include "PDWPersistentUser.generated.h"

UCLASS()
class PDW_API UPDWPersistentUser : public UNebulaFlowPersistentUser
{
	GENERATED_BODY()

public:

	UPDWPersistentUser(const FObjectInitializer& ObjectInitializer);
		
	static const int GAMEPROGRESSION_SLOTS_NUM;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	static UPDWPersistentUser* GetCurrentOwnerUser(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	FGameProgressionSlot& GetCurrentUsedProgressionSlot(bool& Success);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	FGameProgressionSlot& GetGameProgressionSlotByIndex(int inIndex, bool& Success);

	FGameProgressionSlot& GetLastProgressionSlot();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void RemoveProgressionSlotByIndex(int32 InIndex, UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SetCurrentUseSlotIndex(int32 InIndex) { CurrentUsedSlotIndex = InIndex; }

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	int32 GetCurrentUseSlotIndex() const { return CurrentUsedSlotIndex; }
	
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool GetFirstBoot(UObject* WorldContextObject);
	
	int32 GetTotalSlotNum() const { return GAMEPROGRESSION_SLOTS_NUM;}

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void InitializeNewSlot(FGameProgressionSlot& InSlot, UObject* WorldContext);

	void InitSaves(UNebulaFlowGameInstance* InInstance) override;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	TArray<FGameProgressionSlot>& GetGameProgressionSlots() { return GameProgressionSlots; }

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool HasSaveGames();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	TMap<EGameOptionsId, int32>& GetGameOptions() { return GameOptions; }

	UFUNCTION()
	void OnBeforeAutoSave();

	UFUNCTION()
	void OnPostLoadFromSlot(int32 SlotID);

	UFUNCTION()
	void OnProfilePostLoad();
		
	bool GetActivityCompletedOnce() const;

	void SetActivityCompletedOnce(const bool bInCompleted);

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveData")
	bool bIsFirstStartedActivity = false;
		
protected:

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "SaveData")
	TMap<EGameOptionsId, int32> GameOptions{};

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SaveGame")
	TArray<FGameProgressionSlot> GameProgressionSlots;

	UPROPERTY(SaveGame)
	int32 CurrentUsedSlotIndex = -1;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "SaveData")
	bool bActivityCompleted = false;
};
