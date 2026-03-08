// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "FlowDeveloperSettings.generated.h"

class UFlowBasePage;
class UInputMappingContext;
class UNebulaFlowBaseNavbar;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(FlowFSMPages);
/**
 *
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Flow Settings"))
class PDW_API UFlowDeveloperSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
public:
	static UFlowDeveloperSettings* Get();

#pragma region FSM Flow Definitions
protected:

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMGameplayTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMPauseGameTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMCutsceneTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMSkipCutsceneTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMDialogueTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMMinigameSetupTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMMinigameTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMMinigameSucceedTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMMinigameCancelledTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMMinigameCompletedTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMRewardTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMSwitcherTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMQuestMessageTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMLoadUserTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMProceedTag;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMBackTag;
	
	//UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	//FGameplayTag FSMNewGameTag;
	//
	//UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	//FGameplayTag FSMLoadGameTag;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMContinueGameTag;
	
	//UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	//FGameplayTag FSMSlotClickedTag;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMSettingsTag;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMCreditsTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMInGameCreditsTag;

	//UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	//FGameplayTag FSMQuitTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMCustomizationTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMDinoCustomizationTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMMainMenuTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMTeleportTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMRyderMenuTag;

	UPROPERTY(EditDefaultsOnly, Config, Category = "FSM Flow Definitions|States")
	FGameplayTag FSMReplayMinigamesTag;


public:

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetGameplayTag() { return Get()->FSMGameplayTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetPauseGameTag() { return Get()->FSMPauseGameTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetCutsceneTag() { return Get()->FSMCutsceneTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetSkipCutsceneTag() { return Get()->FSMSkipCutsceneTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetDialogueTag() { return Get()->FSMDialogueTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameSetupTag() { return Get()->FSMMinigameSetupTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameTag() { return Get()->FSMMinigameTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameSucceedTag() { return Get()->FSMMinigameSucceedTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameCancelledTag() { return Get()->FSMMinigameCancelledTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameCompletedTag() { return Get()->FSMMinigameCompletedTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetRewardTag() { return Get()->FSMRewardTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetSwitcherTag() { return Get()->FSMSwitcherTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetQuestMessageTag() { return Get()->FSMQuestMessageTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetLoadUserTag() { return Get()->FSMLoadUserTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetProceedTag() { return Get()->FSMProceedTag; };
	
	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetBackTag() { return Get()->FSMBackTag; };
	
	//UFUNCTION(BlueprintPure)
	//static const FGameplayTag GetNewGameTag() { return Get()->FSMNewGameTag; };
	
	//UFUNCTION(BlueprintPure)
	//static const FGameplayTag GetLoadGameTag() { return Get()->FSMLoadGameTag; };
	
	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetContinueTag() { return Get()->FSMContinueGameTag; };
	
	//UFUNCTION(BlueprintPure)
	//static const FGameplayTag GetSlotClickedTag() { return Get()->FSMSlotClickedTag; };
		
	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetSettingsTag() { return Get()->FSMSettingsTag; };
	
	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetCreditsTag() { return Get()->FSMCreditsTag; };
	
	//UFUNCTION(BlueprintPure)
	//static const FGameplayTag GetQuitTag() { return Get()->FSMQuitTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetCustomizationTag() { return Get()->FSMCustomizationTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetDinoCustomizationTag() { return Get()->FSMDinoCustomizationTag; };

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMainMenuTag() { return Get()->FSMMainMenuTag; }

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetTeleportTag() { return Get()->FSMTeleportTag; }

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetRyderMenuTag() { return Get()->FSMRyderMenuTag; }

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetReplayMinigamesTag() { return Get()->FSMReplayMinigamesTag; }

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetInGameCreditsTag() { return Get()->FSMInGameCreditsTag; };

#pragma endregion
};
