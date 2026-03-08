// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Managers/NebulaFlowCheatManager.h"
#include "GameplayTagContainer.h"
#include "PDWCheatManager.generated.h"

class UNebulaFlowDialog;
class UFlowNode;
class UPDWFlowNode_Quest;
class UFlowComponent;

/**
 * 
 */
UCLASS()
class PDW_API UPDWCheatManager : public UNebulaFlowCheatManager
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	FGameplayTag QuestTagPrefix;

	UPROPERTY(EditAnywhere)
	FGameplayTag MinigameTagPrefix;

	UPROPERTY(EditAnywhere)
	FGameplayTag DinoPenTagPrefix;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag DinoNeedTagPrefix;

	//SAVES
	UFUNCTION(Exec)
	void PDW_TestSave();

	UFUNCTION(Exec)
	void PDW_TestLoad(int32 SlotIndex);

	UFUNCTION(exec)
	void PDW_PostLoadTest(int UserID);

	UFUNCTION(exec)
	void PDW_ShowCurrentSlotID();

	int32 CurrentLoadedSlotID = -1;
		
	UFUNCTION(exec)
	void PDW_SaveAndRestore();
	
	//QUESTS

	UFUNCTION(exec)
	void PDW_EndCurrentQuest();

	UFUNCTION(exec)
	void PDW_EndCurrentStep();

	UFUNCTION(exec)
	void PDW_StartQuest(FString _QuestToStart);

	UFUNCTION(exec)
	void PDW_GoToQuestMinigame(FString _MinigameTag, FString _QuestTag, float TeleportHeight = 100.f);

	//PALEO CENTER

	UFUNCTION(exec)
	void PDW_UnlockDinoPen(FString _PenToUnlock);

	UFUNCTION(exec)
	void PDW_UnlockAllDinoPens();

	UFUNCTION(exec)
	void PDW_ChangeDinoPenActivity(FString _PenTag, bool IsActive);

	UFUNCTION(exec)
	void PDW_ForceNavMeshRebuild();

	UFUNCTION(exec)
	void PDW_ForceDinoNeed(FString _PenTag, FString NeedTag);

	UFUNCTION(exec)
	void PDW_UnlockEggs(FString _PenTag);

	//PLAYER

	UFUNCTION(exec)
	void PDW_AddItemToInventory(FGameplayTag ItemTag, int32 Quantity);

	UFUNCTION(exec)
	void PDW_AddExp(int32 ExpToAdd);

	UFUNCTION(exec)
	void PDW_GetAllUnlockables();

	UFUNCTION(exec)
	void PDW_SetBreadcrumbsDebug(bool IsActive);

	UFUNCTION(exec)
	void PDW_SetWaypointsDebug(bool IsActive);

	UFUNCTION(exec)
	void PDW_UnlockAllAreas();

	//UI

	UFUNCTION(exec)
	void TestGameModal();

	UNebulaFlowDialog* GameTestDialog = nullptr;

	UFUNCTION(exec)
	void TestSystemModal();

	UNebulaFlowDialog* SystemTestDialog = nullptr;

	UFUNCTION(exec)
	void PDW_ShowGameUI();

	UFUNCTION(exec)
	void PDW_HideGameUI();
	
	UFUNCTION(exec)
	void PDW_HideBuildInfo();
	
	UFUNCTION(exec)
	void PDW_ShowBuildInfo();

	//MAP
	UFUNCTION(exec)
	void PDW_TeleportToShores();

	UFUNCTION(exec)
	void PDW_TeleportToJungle();

	UFUNCTION(exec)
	void PDW_TeleportToDinoPlains();

	UFUNCTION(exec)
	void PDW_TeleportToVolcano();

	UFUNCTION(exec)
	void PDW_TeleportToTutorial();

	//GENERAL
	
	UFUNCTION(exec)
	void PDW_CheckP2Spawns();
	
	UFUNCTION(exec)
	void PDW_ToggleMultiplayer();
	
	UFUNCTION(exec)
	void PDW_MultipleToggleMultiplayer();

	UFUNCTION(exec)
	void PDW_SetNPCActivity(bool IsActive);

	UFUNCTION(exec)
	void PDW_DestroyNPCs();
	
	UFUNCTION(exec)
	void EnableFreeCamera();
	
	UFUNCTION(exec)
	void DisableFreeCamera();

	UFUNCTION(exec)
	void PDW_ReplayMinigame(FString _MinigameTag, FString _QuestTag);

private:

	void Teleport_Internal(FGameplayTag inLocationTag);

	UFUNCTION()
	void ReachMinigameLocation(FGameplayTagContainer MinigameTag, float TeleportHeight);

	UFUNCTION()
	void ResumeQuestFlow(UPDWFlowNode_Quest* QuestNode, TArray<UFlowNode*> NodesToSkip);

	UFUNCTION()
	TArray<UFlowNode*> GetNodesToSkip(UPDWFlowNode_Quest* QuestNode, UClass* TargetNodeClass, bool &bFound, FGameplayTagContainer TagToFind = FGameplayTagContainer::EmptyContainer);

	UFUNCTION()
	void ClearBreadcrumbs();

	UFUNCTION()
	FGameplayTag BuildFullTag(FString CurrentTag, FGameplayTag& Prefix);

	FTimerHandle PlayerToggleTimerHandle;
	
};
