// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Data/PDWGameplayStructures.h"
#include "Data/PDWSaveStructures.h"
#include "PDWDataFunctionLibrary.generated.h"

class UPDWPersistentUser;

/**
 * 
 */
UCLASS()
class PDW_API UPDWDataFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	//SYSTEM
	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static UPDWPersistentUser* GetPersistentUser(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static bool GetIsNewGame(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetIsNewGame(const bool IsNewGame, UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static bool GetIsFirstBoot(UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetIsFirstBoot(UObject* WorldContext,const bool IsFirstBoot);
	
	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static UFlowSaveGame* GetFlowSaveGame(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void SetFlowSaveGame(UObject* WorldContext, UFlowSaveGame* FlowSaveGame);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static bool SaveGame(UObject* WorldContext);
	//PLAYER
	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static FTransform GetPlayerPosition(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void SetPlayerPosition(UObject* WorldContext,const FTransform& Position);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static FGameplayTag GetSelectedPup(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void SetSelectedPup(UObject* WorldContext,const FGameplayTag& Pup);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static bool GetIsOnVehicle(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void SetIsOnVehicle(UObject* WorldContext,const bool IsOnVehicle);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void AddExpPoints(UObject* WorldContext,const int32 AddedPoints);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static int32 GetExpPoints(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static FCustomizationData GetCustomizationData(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetCustomizationData(UObject* WorldContext, FCustomizationData CustomizationData);

	//WORLD
	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static TArray<int32> GetCollectedTreats(UObject* WorldContext);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void AddCollectedTreat(UObject* WorldContext,const int32 TreatId);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void ChangeDinoPenActivity(UObject* WorldContext, FGameplayTag DinoPenTag, bool IsActive);
	
	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static bool IsDinoPenActive(UObject* WorldContext, FGameplayTag DinoPenTag);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static FGameplayTag GetDinoNeed(UObject* WorldContext, FGameplayTag DinoPenTag);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static FGameplayTag GetFakeDinoNeed(UObject* WorldContext, FGameplayTag DinoPenTag);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static FGameplayTag GetDinoCustomization(UObject* WorldContext, FGameplayTag DinoPenTag);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void SetDinoCustomization(UObject* WorldContext, FGameplayTag DinoPenTag, FGameplayTag CustomizationTag);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void SetDinoNeed(UObject* WorldContext, FGameplayTag DinoPenTag, FGameplayTag DinoNeed);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void SetFakeDinoNeed(UObject* WorldContext, FGameplayTag DinoPenTag, FGameplayTag DinoNeed);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void AddItemToInventory(UObject* WorldContext, const FGameplayTag ItemTag, int32 Quantity);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void UpdateItemQuantity(UObject* WorldContext, const FGameplayTag ItemTag, int32 Quantity);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static bool RemoveItemFromInventory(UObject* WorldContext, const FGameplayTag ItemTag);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static bool RemoveItemQuantityFromInventory(UObject* WorldContext, const FGameplayTag ItemTag, int32 Quantity);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static int32 GetItemQuantityFromInventory(UObject* WorldContext, const FGameplayTag ItemTag);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static TMap<FGameplayTag, int32> GetInventory(UObject* WorldContext);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static TMap<FGameplayTag,int32> GetItemsByFilter(UObject* WorldContext, const FGameplayTag Filter);

	static void SetDataLayerState(UObject* WorldContext, TObjectPtr<UDataLayerAsset> DL, bool IsActive);

	static TMap<TObjectPtr<UDataLayerAsset>, bool> GetDataLayersState(UObject* WorldContext);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static float GetStationFood(UObject* WorldContext, const FGameplayTag DinoPen);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static float GetStationWater(UObject* WorldContext, const FGameplayTag DinoPen);


	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void UpdateFoodInStation(UObject* WorldContext, const FGameplayTag DinoPen, int32 NewQuantity);

	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static void UpdateWaterInStation(UObject* WorldContext, const FGameplayTag DinoPen, float NewQuantity);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void SetFact(UObject* WorldContext,const FGameplayTag& Fact,const bool bValue);

	UFUNCTION(BlueprintPure, Category = "", meta = (WorldContext = "WorldContextObject"))
	static bool GetFact(UObject* WorldContext,const FGameplayTag& Fact);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void SetInteractableState(UObject* WorldContext, const int32 InteractableID, const FInteractableData& InteractableData);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static FInteractableData GetInteractableState(UObject* WorldContext, const int32 InteractableID);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void RemoveInteractableStateByIdentityTags(UObject* WorldContext, const FGameplayTagContainer IdentityTags);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
	static void RemoveInteractableStateByIndex(UObject* WorldContext, const int32 InteractableID);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void AddUsedInteractable(UObject* WorldContext,const int32 InteractableID);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static bool IsInteractableUsed(UObject* WorldContext, int32 InteractableID);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static int32 GetUsedInteractableCount(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static TArray<FGameplayTag> GetVisitedAreaIds(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static bool IsAreaVisited(UObject* WorldContextObject, const FGameplayTag& RequestedAreaId);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGameplayTag GetPlayerAreaId(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetPlayerAreaId(UObject* WorldContextObject, const FGameplayTag& CurrentAreaId);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetPlayerAreaIdByTeleportId(UObject* WorldContextObject, const FGameplayTag& TeleportId);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static TArray<FQuestData> GetCompletedQuests(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void AddCompletedQuest(UObject* WorldContextObject, FQuestData CompletedQuest);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<FPDWMinigameInfo> GetPlayedMinigamesInfo(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void AddPlayedMinigame(UObject* WorldContextObject, FPDWMinigameInfo PlayedMinigame);

	static void SetCurrentQuestDescription(UObject* WorldContextObject, const FText& QuestDescription);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void IncrementPupTreatsCollected(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static int32 GetPupTreatsCollected(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void IncrementDinoNeedsSolved(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static int32 GetDinoNeedsSolved(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void IncrementOpenedChests(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static int32 GetOpenedChests(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void AddDisabledInput(UObject* WorldContextObject, FName InputAction);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void RemoveDisabledInput(UObject* WorldContextObject, FName InputAction);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static TSet<FName> GetDisabledInputs(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetAllowedPups(UObject* WorldContextObject, FGameplayTagContainer AllowedPups);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void ClearAllowedPups(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static FGameplayTagContainer GetAllowedPups(UObject* WorldContextObject);
};
