// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "PDWPupConfigurationAsset.h"
#include "UI/HUD/PDWGameplayHUD.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Managers/PDWHUDSubsystem.h"
#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "UI/Widgets/PDWMinigameNavButton.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "PDWGameSettings.generated.h"

/**
 * 
 */
class APDWVehiclePawn;
class UPDWMinigameNavButton;
class USettingsDefinitionDataAsset;

UENUM(BlueprintType)
enum class EBaseSkillType : uint8
{
	SwapPup = 0,
	ToggleVehicle = 1,
	InteractionBase = 2,
	Jump =3,
	VehicleSkill = 4,
};

USTRUCT(BlueprintType)
struct FPDWAreaInfo {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText AreaName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "ID.Teleport"))
	FGameplayTag TeleportLocationId{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> AreaIcon{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> PreviewImage{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "ID.DinoPen"))
	FGameplayTag DinoPenId{ FGameplayTag::EmptyTag };
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "GameSettings"))
class PDW_API UPDWGameSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static UPDWGameSettings* Get();


#pragma region UIActionsDefinitions
protected:

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName UIConfirm = "Confirm";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName UIBack = "Back";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName UIDelete = "Delete";
		
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName UIReset = "Reset";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName UIResetInputs = "ResetInputs";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName UIRemap = "Remap";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName ChangePup = "ChangePup";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName BackToGameplay = "BackToGameplay";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName CloseSwapCharacter = "CloseSwapCharacter";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName Join2Player = "Join2Player";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName GuestStreamPlay = "GuestStreamPlay";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName SuccedAnimationCompleted = "SuccedAnimationCompleted";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName JoinPlayer2Succes = "JoinPlayer2Succes";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName JoinPlayer2Fail = "JoinPlayer2Fail";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName StartConversation = "StartConversation";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName EndConversation = "EndConversation";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName TriggerConversationFromSequence = "ConversationFromSequence";
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName Skip = "Skip";
		
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName Continue = "Continue";
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName LoadGame = "LoadGame";
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName NewGame = "NewGame";
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName SelectSlot = "SelectSlot";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName Settings = "Settings";
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName Credits = "Credits";
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName Quit = "Quit";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName ScrollEula = "ScrollEula";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName ChangeCustomization = "ChangeCustomization";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName EquipNow = "EquipNow";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName ToggleVehiclePup = "ToggleVehiclePup";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName PrevTab = "PrevTab";

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "UI|UI Actions Definitions")
	FName NextTab = "NextTab";


public:
	
	UFUNCTION(BlueprintPure)
	static const FName GetUIActionDialogueFromSequence() {return Get()->TriggerConversationFromSequence;}; 

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionConfirm() {return Get()->UIConfirm;};
	
	UFUNCTION(BlueprintPure)
	static const FName GetUIActionBack() { return Get()->UIBack; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionDelete() { return Get()->UIDelete; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionReset() { return Get()->UIReset; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionResetInputs() { return Get()->UIResetInputs; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionRemap() { return Get()->UIRemap; };
	
	UFUNCTION(BlueprintPure)
	static const FName GetActionChangePup() { return Get()->ChangePup; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionBackToGameplay() { return Get()->BackToGameplay; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionCloseSwapCharacter() { return Get()->CloseSwapCharacter; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionJoin2Player() { return Get()->Join2Player; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionGuestStreamPlay() { return Get()->GuestStreamPlay; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionSuccedAnimationCompleted() { return Get()->SuccedAnimationCompleted; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionJoinPlayer2Succes() { return Get()->JoinPlayer2Succes; };

	UFUNCTION(BlueprintPure)
	static const FName GetActionJoinPlayer2Fail() { return Get()->JoinPlayer2Fail; };

	UFUNCTION(BlueprintPure)
	static const FName GetStartConversationAction() { return Get()->StartConversation; };

	UFUNCTION(BlueprintPure)
	static const FName GetEndConversationAction() { return Get()->EndConversation; };

	UFUNCTION(BlueprintPure)
	static const FName GetSkipAction() { return Get()->Skip; };
	
	UFUNCTION(BlueprintPure)
	static const FName GetUIActionContinue() { return Get()->Continue; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionLoadGame() { return Get()->LoadGame; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionNewGame() { return Get()->NewGame; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionSelectSlot() { return Get()->SelectSlot; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionSettings() { return Get()->Settings; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionCredits() { return Get()->Credits; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionQuit() { return Get()->Quit; };
	
	UFUNCTION(BlueprintPure)
	static const FName GetUIActionScrollEula() { return Get()->ScrollEula; };

	UFUNCTION(BlueprintPure)
	static const FName GetCustomizationAction() { return Get()->ChangeCustomization; };

	UFUNCTION(BlueprintPure)
	static const FName GetEquipNowAction() { return Get()->EquipNow; };

	UFUNCTION(BlueprintPure)
	static const FName GetToggleVehiclePupAction() { return Get()->ToggleVehiclePup; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionPrevTab() { return Get()->PrevTab; };

	UFUNCTION(BlueprintPure)
	static const FName GetUIActionNextTab() { return Get()->NextTab; };

	UFUNCTION(BlueprintCallable)
	static const FPDWAreaInfo GetAreaInfo(const FGameplayTag& AreaId) { return Get()->GameAreasInfo.FindRef(AreaId); };
	
	UFUNCTION(BlueprintCallable)
	static const TMap<FGameplayTag, FPDWAreaInfo>& GetAllAreaInfo() { return Get()->GameAreasInfo; };


#pragma endregion

#pragma region HUD
protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "UI|HUD")
	TMap<EHUDType,TSubclassOf<UPDWBaseHUD>> HUDClasses {};

	UPROPERTY(Config, EditDefaultsOnly, Category = "UI|HUD|NavIcon")
	TSubclassOf<UNebulaFlowNavbarButton> NavButtonClassReference;

	UPROPERTY(Config, EditDefaultsOnly, Category = "UI|HUD|NavIcon")
	TSubclassOf<UPDWMinigameNavButton> MinigameNavButtonClassReference;

public:
	UFUNCTION(BlueprintCallable)
	static TSubclassOf<UPDWBaseHUD> GetHUDClass(EHUDType HudType);

	UFUNCTION(BlueprintCallable)
	static TSubclassOf<UNebulaFlowNavbarButton> GetNavButtonClassReference();

	UFUNCTION(BlueprintCallable)
	static TSubclassOf<UPDWMinigameNavButton> GetMinigameNavButtonClassReference();
#pragma endregion

#pragma region InputConfig

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT", meta = (ForceInlineRow, AllowedClasses = "/Script/PDW.InputMappingContext"))
	TMap<FGameplayTag, FSoftObjectPath> ListOfMappingContextByTag;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT", meta = (ForceInlineRow, AllowedClasses = "/Script/PDW.InputMappingContext"))
	TMap<FGameplayTag, FSoftObjectPath> ListOfMappingContextByTagSwitchSingleJoycon;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTag InteractTagMap;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTag UIDefaultTagMap;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTag DefaultGameplayTagMap;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTag DefaultMinigameTagMap;
	
	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTag ExitMinigameActiontag;
	
	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTag SkipMinigameActiontag;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT", meta = (ForceInlineRow, AllowedClasses = "/Script/PDW.InputAction"))
	TMap<FGameplayTag, FSoftObjectPath> ListOfInputActions;

	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	FGameplayTagContainer FaceButtonsList;
	UPROPERTY(Config, EditDefaultsOnly, Category = "INPUT")
	TMap<FGameplayTag,FGameplayTag> HoldMap;

public:
	UFUNCTION(BlueprintCallable)
	static UInputMappingContext* GetMappinContextByTag(const FGameplayTag& MapTag);

	UFUNCTION(BlueprintCallable)
	static UInputAction* GetInputActionByTag(const FGameplayTag& ActionTag);

	UFUNCTION(BlueprintCallable)
	bool GetWrongFaceButtons(const FGameplayTag& CorrectButton,FGameplayTagContainer& WrongButtons);

	UFUNCTION(BlueprintCallable)
	static FGameplayTagContainer GetFaceButtonList() {return Get()->FaceButtonsList; };

	UFUNCTION(BlueprintCallable)
	static const FGameplayTag GetInteractionTagMap() { return Get()->InteractTagMap; };

	UFUNCTION(BlueprintCallable)
	static const FGameplayTag GetUIDefaultTagMap() { return Get()->UIDefaultTagMap; };

	UFUNCTION(BlueprintCallable)
	static const FGameplayTag GetDefaultGameplayTagMap() { return Get()->DefaultGameplayTagMap; };

	UFUNCTION(BlueprintCallable)
	static const FGameplayTag GetDefaultMinigameTagMap() { return Get()->DefaultMinigameTagMap; };

	UFUNCTION(BlueprintCallable)
	static const FGameplayTag GetExitMinigameActionTag() { return Get()->ExitMinigameActiontag; };

	UFUNCTION(BlueprintCallable)
	static const FGameplayTag GetSkipMinigameActionTag() { return Get()->SkipMinigameActiontag; };
#pragma endregion

protected:

	UPROPERTY(Config, EditAnywhere, Category = "Game Areas Settings")
	TMap<FGameplayTag, FPDWAreaInfo> GameAreasInfo{};

	UPROPERTY(Config, EditAnywhere, Category = "Characters Settings",meta = (ForceInlineRow,AllowedClasses = "/Script/PDW.PDWPupConfigurationAsset"))
	TMap<FGameplayTag,FEntity> EntityConfig{};

	UPROPERTY(Config, EditAnywhere, Category = "Characters Settings",meta = (ForceInlineRow))
	TMap<EBaseSkillType, TSubclassOf<UPDWGASAbility>> BasicSkills = {};

	UPROPERTY(Config, EditAnywhere, Category = "MiniGame Settings")
	FName MiniGameComponentTag = "MiniGameComponent";

	UPROPERTY(Config, EditAnywhere, Category = "MiniGame Settings")
	FName MiniGameBehaviourPreventTag = "BehaviourStarted";

	UPROPERTY(Config, EditAnywhere, Category = "MiniGame Settings")
	FName MiniGameStartingPositionTag = "PlayerStart_";

	UPROPERTY(Config, EditAnywhere, Category = "MiniGame Settings")
	FName MiniGameMovingMeshTag = "MiniGameMovingMesh";

	UPROPERTY(Config, EditAnywhere, Category = "MiniGame Settings")
	FGameplayTag MinigameExitNotifyFlowTag;;

	UPROPERTY(Config, EditAnywhere, Category = "MiniGame Settings")
	FGameplayTag MinigameSkipNotifyFlowTag;

	UPROPERTY(Config, EditAnywhere, Category = "CollisionSettings")
	FCollisionProfileName InteractionReceiverProfileName;

	UPROPERTY(Config, EditAnywhere, Category = "CollisionSettings")
	FCollisionProfileName InteractionProfileName;

	UPROPERTY(Config, EditAnywhere, Category = "CollisionSettings")
	FCollisionProfileName AutoSwapAreaProfileName;

	UPROPERTY(Config, EditAnywhere, Category = "CollisionSettings")
	FCollisionProfileName ExclusionVolumeProfileName;

	UPROPERTY(Config, EditAnywhere, Category = "CollisionSettings")
	FCollisionProfileName OutlineCollisionProfileName;

	UPROPERTY(Config, EditAnywhere, Category = "CollisionSettings")
	FCollisionProfileName AudioEnvironmentAreaProfileName;



public:

	UFUNCTION(BlueprintPure)
	static void GetEntityConfig(TMap<FGameplayTag, FEntity>& OutConfig);

	UFUNCTION(BlueprintPure)
	static FEntity GetEntityConfigForTag(const FGameplayTag& EntityTag);

	UFUNCTION(BlueprintPure)
	static TSubclassOf<APDWCharacter> GetPupClass(const FGameplayTag& inTag);

	UFUNCTION(BlueprintPure)
	static TSubclassOf<APDWVehiclePawn> GetPupVehicleClass(const FGameplayTag& inTag);

	UFUNCTION(BlueprintPure)
	static TSubclassOf<UPDWGASAbility> GetBasicSkill(const EBaseSkillType& inSkillType);

	UFUNCTION(BlueprintPure)
	static TSubclassOf<UPDWGASAbility> GetEntitySkill(const FGameplayTag& inPupTag);

	UFUNCTION(BlueprintPure)
	static FGameplayTagContainer& GetEntitySkillTypeContainer(const FGameplayTag& inPupTag);

	UFUNCTION(BlueprintPure)
	static FGameplayTagContainer& GetEntityInteractionsContainer(const FGameplayTag& inPupTag);

	UFUNCTION(BlueprintPure)
	static const int32 GetAllPupTags(TArray<FGameplayTag>& outPupsTags);

	UFUNCTION(BlueprintPure)
	static const FPupUIInfo& GetPupInfo(const FGameplayTag& inPupTag);

	UFUNCTION(BlueprintPure)
	static const FPupCustomizationInfo& GetPupCustomizationInfo(const FGameplayTag& inPupTag);

	UFUNCTION(BlueprintPure)
	static const FName GetMiniGameComponentTag();

	UFUNCTION(BlueprintPure)
	static const FName GetMiniGameStartintPositionTag();

	UFUNCTION(BlueprintPure)
	static const FName GetMiniGameBehaviourPreventTag();

	UFUNCTION(BlueprintPure)
	static const FName GetMiniGameMovingMeshTag();

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameExitNotifyFlowTag() {return Get()->MinigameExitNotifyFlowTag;};

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetMinigameSkipNotifyFlowTag() {return Get()->MinigameSkipNotifyFlowTag;};

	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetInteractionReceiverProfileName();
	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetInteractionProfileName();
	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetAutoSwapAreaProfileName();

	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetExclusionVolumeProfileName();

	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetOutlineCollisionProfileName();

	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetAudioEnvironmentAreaProfileName();

private:

	UPROPERTY()
	TMap<FGameplayTag, UPDWPupConfigurationAsset*> EntityConfigMap{};

};
