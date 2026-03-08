// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/PDWPersistentUser.h"
#include "StructUtils/StructView.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequenceActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PDWGameplayFunctionLibrary.generated.h"

class APDWCharacter;
class APDWGameplayGameMode;
class APDWPlayerController;
class APlayerController;
class UFlowAsset;
class UPDWUIManager;
class UInputAction;
class UFlowComponent;
class UPDWAchievementManager;

/**
 *
 */
UCLASS()
class PDW_API UPDWGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWGameplayGameMode* GetPDWGameplayGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWPlayerController* GetPlayerControllerOne(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWPlayerController* GetPlayerControllerTwo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static ANebulaFlowPlayerController* GetNebulaPlayerControllerOne(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static ANebulaFlowPlayerController* GetNebulaPlayerControllerTwo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWCharacter* GetPlayerOne(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWCharacter* GetPlayerTwo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static TArray<APDWCharacter*> GetPlayers(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWPlayerState* GetPlayerStateOne(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWPlayerState* GetPlayerStateTwo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWCharacter* GetPlayerByIndex(const UObject* WorldContextObject, const int32 Index);

	UFUNCTION(BlueprintPure, Category = "UTILITY", meta = (WorldContext = "WorldContextObject"))
	static const FTransform GetSocketTransformFromSpecificMesh(const AActor* inOwner, const FName& inSocketName, const FName& inMeshTag = NAME_None);

	UFUNCTION(BlueprintPure, Category = "UTILITY")
	static const bool IsSecondPlayer(const APlayerController* PlayerControllerToVerify);

	UFUNCTION(BlueprintPure, Category = "UTILITY")
	static const bool IsSecondPlayerByActor(const UObject* WorldContextObject, const AActor* ActorToVerify);

	UFUNCTION(BlueprintPure, Category = "UTILITY", meta = (WorldContext = "WorldContextObject"))
	static const bool IsMultiplayerOn(const UObject* WorldContextObject);

	static const void SetDataLayerRuntimeState(UObject* WorldContextObject, TMap<TObjectPtr<UDataLayerAsset>, bool> DataLayers,  bool SaveIt = true);

	UFUNCTION(BlueprintCallable)
	static const void BP_SetDataLayerRuntimeState(UObject* WorldContextObject, TMap<UDataLayerAsset*, bool> DataLayers,  bool SaveIt = true);

	UFUNCTION(BlueprintPure, Category = "QuestFlow")
	static bool IsNextNodeChangeState(UFlowAsset* FlowAsset);

	UFUNCTION(BlueprintCallable,Category = "QuestFlow")
	static void TriggerQuestTalkingEvent(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SmartObjects")
	static void SendSmartObjectEvent(const UObject* WorldContextObject, const FSmartObjectSlotHandle SlotHandle, const FGameplayTag EventTag, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Items")
	static void AddItemsToInventory(UObject* WorldContextObject, TMap<FGameplayTag,int32> Items, bool Notify = true);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static ULevelSequencePlayer* PDWCreateLevelSequence(UObject* WorldContextObject, ULevelSequence* InLevelSequence, FMovieSceneSequencePlaybackSettings Settings, ALevelSequenceActor*& OutActor);

	UFUNCTION(Blueprintpure, Category = "Managers")
	static UPDWUIManager* GetPDWUIManager(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Graphics")
	static UTextureRenderTarget2D* PDWCreateRenderTarget2D(UObject* WorldContextObject, int32 Width, int32 Height, ETextureRenderTargetFormat Format, FLinearColor ClearColor, bool bAutoGenerateMipMaps, bool bSupportUAVs);
	// player
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UPDWPersistentUser* GetPDWPersistentUser(UObject* WorldContextObject);

	// player
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static FGameProgressionSlot& GetCurrentProgressionSlot(UObject* WorldContextObject, bool& WasSuccessful);

	//Dialogues

	UFUNCTION(BlueprintCallable)
	static void TriggerSequenceDialogue(UObject* WorldContextObject);

	//Camera Shake and Force Feedback

	UFUNCTION(BlueprintCallable)
	static void StartCameraShake(APlayerController* PlayerController, TSubclassOf<UCameraShakeBase> ShakeClass);

	UFUNCTION(BlueprintCallable)
	static void StopCameraShake(APlayerController* PlayerController, TSubclassOf<UCameraShakeBase> ShakeClass, bool bImmediately = true);

	UFUNCTION(BlueprintCallable)
	static void PlayForceFeedback(TArray<APDWPlayerController*> PlayerControllers, FGameplayTag ForceFeedbackTag, FName Tag = NAME_None, bool bLooping = false, bool bIgnoreTimeDilation = false, bool bPlayWhilePaused = false);

	UFUNCTION(BlueprintCallable)
	static void PlayForceFeedbackBothPlayers(UObject* WorldContextObject, FGameplayTag ForceFeedbackTag, FName Tag = NAME_None, bool bLooping = false, bool bIgnoreTimeDilation = false, bool bPlayWhilePaused = false);

	UFUNCTION(BlueprintCallable)
	static void StopForceFeedback(TArray<APDWPlayerController*> PlayerController);

	UFUNCTION(BlueprintCallable)
	static void StopForceFeedbackBothPlayers(UObject* WorldContextObject);

	static bool GetRandomSpawnLocationNear(UWorld* World, FVector& Location, AActor* ActorToIgnore = nullptr, bool bP2 = false);
	static bool IsPointInNavMesh(UWorld* World, const FVector& Point, float SearchRadius = 50.f);

	static FVector GetValidSpawnPointNear(UWorld* World1, FVector Origin, float Radius = 500.f);
	static bool IsSpawnLocationClear(UWorld* World1, FVector Location, float Radius = 200.f, AActor* ActorToIgnore = nullptr, bool bP2 = false);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void AddDisabledInputAction(UObject* WorldContextObject, FName InputAction);	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void RemoveDisabledInputAction(UObject* WorldContextObject, FName InputAction);
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static bool IsInputActionDisabled(UObject* WorldContextObject,  UInputAction* InputAction);
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void SetVehiclesPhysiscs(UObject* WorldContextObject, bool IsActive);

	// QUALITY SETTINGS
	static void ApplyQualitySettings(UObject* WorldContextObject, const FString SectionName);
	static void RestoreQualitySettings(UObject* WorldContextObject, const FString SectionName);
	// ----------------

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static TArray<FKey> FindAllKeysForAction(UObject* WorldContextObject, UInputAction* Action);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static TArray<FQuestData> GetAreaQuests(UObject* WorldContextObject, FGameplayTag AreaID);

	static UFlowComponent* GetQuestManagerComp(UObject* WorldContextObject);
	
	template <typename T>
	FORCEINLINE static void ShuffleArray(TArray<T>& ArrayToShuffle)
	{
		const int32 LastIndex = ArrayToShuffle.Num() - 1;
		for (int32 i = LastIndex; i > 0; --i)
		{
			const int32 RandomIndex = FMath::RandRange(0, i);
			ArrayToShuffle.Swap(i, RandomIndex);
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<FPDWMinigameInfo> GetPlayedMinigames(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static UPDWAchievementManager* GetAchievementManager(UObject* WorldContextObject);
};
