// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "PDWGameplayTagSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "PDWTags"))
class PDW_API UPDWGameplayTagSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static UPDWGameplayTagSettings* Get();

public:

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetMinigameInteractionTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInteractionActionTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInteractionSkillTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInteractionAreaTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetQuestTargetTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInputPressedTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInputReleasedTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetMinigameTransitionToComplete();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetMinigameNotifyOverlap();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetMinigameNotifyDestroy();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetMinigameNotifySpawn();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInteractionDefaultState();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetHappyDino();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetHungryDino();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetHungryDinoWithItem();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetThirstyDino();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetDirtyDino();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetHurtDino();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetBoredDino();

	UFUNCTION(BlueprintPure)
	static TArray<FGameplayTag> GetDinoNeeds();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetSOUseEvent();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetPupTreatTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetAutoSwapRefreshTag();
	
	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetAutoSwapStopRefreshTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetVehicleBoostVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetVehicleImpactVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetVehicleLandVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetPupLandVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetSwapPupVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetChangeToVehicleVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetChangeToPupVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetRewardVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetCollectEggVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetHornVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetBarkVibrationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetTrunkImpactTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInteractionInactiveState();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetPupCustomizationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetDinoCustomizationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetDinoPenCustomizationTag();

	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetEggTag();


protected:

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Skills")
	FGameplayTag Interaction_Minigame;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Skills")
	FGameplayTag Interaction_Action;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Skills")
	FGameplayTag Interaction_Skill;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Skills")
	FGameplayTag Interaction_Area;
	
	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Interaction")
	FGameplayTag Interaction_DefaultState;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Quest")
	FGameplayTag Quest_Target;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Input")
	FGameplayTag Input_Pressed;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Input")
	FGameplayTag Input_Released;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | MiniGame")
	FGameplayTag Minigame_Transition_To_Complete;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | MiniGame")
	FGameplayTag Minigame_Notify_Success;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | MiniGame")

	FGameplayTag Minigame_Notify_Overlap_Success;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | MiniGame")
	FGameplayTag Minigame_Notify_Spawn_Success;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | MiniGame")
	FGameplayTag Minigame_Notify_Destroy_Success;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag HappyDino;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag HungryDino;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag HungryDinoWithItem;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag ThirstyDino;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag DirtyDino;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag HurtDino;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Paleocenter")
	FGameplayTag BoredDino;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | SmartObjects")
	FGameplayTag SOUseEvent;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Collectible")
	FGameplayTag PupTreatTag;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | AutoSwapArea")
	FGameplayTag AutoSwapStopRefreshTag;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | AutoSwapArea")
	FGameplayTag AutoSwapAreaRefreshSkillTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag VehicleBoostVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag VehicleImpactVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag VehicleLandVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag PupLandVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag SwapPupVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag ChangeToVehicleVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag ChangeToPupVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag RewardVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag CollectEggVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag HornVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag BarkVibrationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag TrunkImpactTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag Interaction_InactiveState;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag PupCustomizationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag DinoCustomizationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag DinoPenCustomizationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Definitions | Vibrations")
	FGameplayTag EggTag;

};
