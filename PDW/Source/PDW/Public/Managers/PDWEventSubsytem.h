// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/PDWGameplayStructures.h"
#include "Data/GameOptionsEnums.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "PDWEventSubsytem.generated.h"

class UPDWInteractionReceiverComponent;
class UPDWInteractionBehaviour;
class UFlowComponent;
class UFlowNode;
class UPDWRemappingActionButton;
class APDWEnvironmentAudioArea;

USTRUCT(BlueprintType)
struct PDW_API FInteractionEventSignature
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EventTag;

	UPROPERTY(BlueprintReadWrite)
	AActor* Interacter = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AActor* MinigameActorTarget = nullptr;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPageReady);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHintRequest, const FGameplayTag&, inMinigameID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetHintTimerRequest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestTalkingEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitGameEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayStateEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayStateExit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitcherStateWaitForAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopJumpEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDeactivation, AActor*, DeactivatingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJumpEventOnSpline, const float&, inJumpValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShowHint, USceneComponent*, Owner,const FGameplayTag&, inMinigameID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionExecuted, const FInteractionEventSignature&, inSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutsceneStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutsceneEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemCollected, FGameplayTag, ItemTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEggCollected, FGameplayTag, EggTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoodCollected, FGameplayTag, FoodTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeedCollected, FGameplayTag, SeedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDinoPenCosmeticCollected, FGameplayTag, CosmeticTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEggHatch, FGameplayTag, CosmeticTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPenAreaBeginOverlap, FPDWDinoPenInfo, PenInfo, AActor*, OverlappedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDinoNeedUpdate, FPDWDinoPenNeed, DinoNeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionComplete, UPDWInteractionReceiverComponent*, Interaction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnObservedActorLoaded, AActor*, Actor, UFlowComponent*, Component, FPDWQuestTargetData&, TargetData, UFlowNode*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnObservedActorUnloaded, AActor*, Actor, UFlowComponent*, Component, FPDWQuestTargetData&, TargetData, UFlowNode*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMultiplayerStateChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPlayerAreaChange, const APDWPlayerController* , Player, FGameplayTagContainer, AllowedPups, bool, EnterArea, const APDWAutoSwapArea*, AutoSwapArea);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsApplied);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsChanged, const FName, ChangedSettingId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPenAreaEndOverlap, AActor*, OverlappedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModalResponse, const FString, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDinoPenActivityChange, FGameplayTag, PenTag, bool, IsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameOptionsChanged, EGameOptionsId, SettingsId, int32, CurrentValueIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGrapplingHookTargetAquired,const TArray<AActor*>&, Target,APDWPlayerController*, ControllerSender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotOnFocus,int32, SlotID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRamMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNotifyBind,const FGameplayTag&, ActionToBind, APDWPlayerController*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNotifyUnBind,int32, BindingHandle, APDWPlayerController*, Owner);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionSuccess, const FPDWInteractionPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestInteractionSuccess, const FPDWInteractionPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStateAdd, UPDWInteractionReceiverComponent*, Comp, const FGameplayTag&, PrevTag, const FGameplayTag&, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestInteractionStateAdd, UPDWInteractionReceiverComponent*, Comp, const FGameplayTag&, PrevTag, const FGameplayTag&, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwapCharacterMenuClosed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRockRemoved, AActor*, inRockRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemappingButtonAction, UPDWRemappingActionButton*, ActionButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRemappingButtonAction2, UPDWRemappingActionButton*, ActionButton, FKey, InputKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSimpleEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransitionInEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransitionOutEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinigameCompleteOrLeft,AActor*,Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinigameSpawned, UPDWMinigameConfigComponent*, MinigameComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinRequestRefused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPupCustomize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDinoCustomize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportConfirm);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDinoNeedCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPupTreatCollected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinigameInitialization);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, const FGameplayTag&, QuestID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDinoPenUnlocked, const FGameplayTag&, DinoPenID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPuzzlePosses, bool, bPosses, USceneComponent*, PuzzlePiece);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationEnter, const FGameplayTag&, CurrentAreaId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovingMeshBehaviour, USceneComponent*, SceneComponent);


UCLASS()
class PDW_API UPDWEventSubsytem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	static UPDWEventSubsytem* Get(UObject* WorldContext);
	FOnInteractionExecuted* BindToInteractionEvents(const FGameplayTag& inEventID);
	void TriggerInteractionEvent(const FGameplayTag& inEventID,const FInteractionEventSignature& inSignature);
	
	UFUNCTION()
	void OnPuzzlePossesEvent(bool bPosses,USceneComponent* PuzzlePiece);
	UPROPERTY(BlueprintAssignable)
	FOnPuzzlePosses OnPuzzlePosses;

	UFUNCTION(BlueprintCallable)
	void OnPageReadyEvent();
	UPROPERTY(BlueprintAssignable)
	FOnPageReady OnPageReady;

	void OnJoinRequestRefusedEvent();
	UPROPERTY(BlueprintAssignable)
	FOnJoinRequestRefused OnJoinRequestRefused;

	UFUNCTION(BlueprintCallable)
	void OnRockRemovedEvent(AActor* inRockRemoved);
	FOnRockRemoved OnRockRemoved;

	void OnMinigameCompleteOrLeftEvent(AActor* Actor);
	UPROPERTY(BlueprintAssignable)
	FOnMinigameCompleteOrLeft OnMinigameCompleteOrLeft;

	void OnMinigameInitializationEvent();
	FOnMinigameInitialization OnMinigameInitialization;

	UPROPERTY(BlueprintAssignable)
	FOnTeleportConfirm OnTeleportConfirm;

	UPROPERTY(BlueprintAssignable)
	FOnMinigameSpawned OnMinigameSpawned;

	void OnJumpOnSpline(const float& inJumpValue);
	FOnJumpEventOnSpline OnJumpEventOnSpline;

	void NotifyBindEvent(const FGameplayTag& ActionToBind,APDWPlayerController* Owner);
	FOnNotifyBind OnNotifyBind;

	void NotifyUnBindEvent(int32 BindingHandle, APDWPlayerController* Owner);
	FOnNotifyUnBind OnNotifyUnBind;

	void OnRamModeEvent();
	UPROPERTY(BlueprintAssignable)
	FOnRamMode OnRamMode;

	void OnStopRamEvent();
	UPROPERTY(BlueprintAssignable)
	FOnRamMode OnStopRamMode;

	void OnStopJumpOnSpline();
	FOnStopJumpEvent OnStopJumpEvent;


	UFUNCTION(BlueprintCallable)
	void OnQuestTalkingEvent();
	FOnQuestTalkingEvent OnQuestTalking;

	void OnInitGameEnterEvent();
	FOnInitGameEnter OnInitGameEnter;

	void OnGameplayStateEnterEvent();
	FOnGameplayStateEnter OnGameplayStateEnter;

	void OnGameplayStateExitEvent();
	FOnGameplayStateExit OnGameplayStateExit;

	void OnSwitcherStateWaitForActionEvent();
	FOnSwitcherStateWaitForAction OnSwitcherStateWaitForAction;

	void OnCutsceneStartEvent();
	UPROPERTY(BlueprintAssignable)
	FOnCutsceneStart OnCutsceneStart;

	void OnCutsceneEndEvent();
	UPROPERTY(BlueprintAssignable)
	FOnCutsceneEnd OnCutsceneEnd;
	
	void OnItemCollectedEvent(FGameplayTag ItemTag);
	FOnItemCollected OnItemCollected;

	void OnEggCollectedEvent(FGameplayTag EggTag);
	FOnEggCollected OnEggCollected;
	
	void OnFoodCollectedEvent(FGameplayTag FoodTag);
	FOnFoodCollected OnFoodCollected;

	void OnSeedCollectedEvent(FGameplayTag SeedTag);
	FOnSeedCollected OnSeedCollected;

	void OnDinoPenCosmeticCollectedEvent(FGameplayTag CosmeticTag);
	FOnDinoPenCosmeticCollected OnDinoPenCosmeticCollected;

	void OnEggHatchEvent(FGameplayTag NestTag);
	FOnEggHatch OnEggHatch;
	
	void OnInteractionCompleteEvent(UPDWInteractionReceiverComponent* Interaction);
	FOnInteractionComplete OnInteractionComplete;

	void OnObservedActorLoadedEvent(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator);
	UPROPERTY(BlueprintAssignable)
	FOnObservedActorLoaded OnObservedActorLoaded;

	void OnObservedActorUnloadedEvent(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator);
	UPROPERTY(BlueprintAssignable)
	FOnObservedActorUnloaded OnObservedActorUnloaded;

	UFUNCTION()
	void OnMultiplayerStateChangeEvent();
	UPROPERTY(BlueprintAssignable)
	FOnMultiplayerStateChange OnMultiplayerStateChange;
		
	void OnSettingsAppliedEvent();
	UPROPERTY(BlueprintAssignable)
	FOnSettingsApplied OnSettingsApplied;

	UFUNCTION()
	void OnPlayerAreaChangeEvent(const APDWPlayerController* Player, FGameplayTagContainer AllowedPups, bool EnterArea, const APDWAutoSwapArea* NewArea);

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAreaChange OnPlayerAreaChange;

	void OnPenAreaBeginOverlapEvent(FPDWDinoPenInfo PenInfo, AActor* OverlappedActor);
	FOnPenAreaBeginOverlap OnPenAreaBeginOverlap;

	void OnPenAreaEndOverlapEvent(AActor* OverlappedActor);
	FOnPenAreaEndOverlap OnPenAreaEndOverlap;

	void OnDinoNeedUpdateEvent(FPDWDinoPenNeed NewNeed);
	FOnDinoNeedUpdate OnDinoNeedUpdate;

	void OnGameOptionsChangedEvent(EGameOptionsId SettingsId, int32 CurrentValueIndex);
	UPROPERTY(BlueprintAssignable)
	FOnGameOptionsChanged OnGameOptionsChanged;
	
	void OnModalResponseEvent(const FString Response);
	UPROPERTY(BlueprintAssignable)
	FOnModalResponse OnModalResponse;

	void OnDinoPenActivityChangeEvent(const FGameplayTag PenTag, bool IsActive);
	FOnDinoPenActivityChange OnDinoPenActivityChange;

	void OnInteractionSuccessEvent(const FPDWInteractionPayload& Payload);
	FOnInteractionSuccess OnInteractionSuccess;

	void OnQuestInteractionSuccessEvent(const FPDWInteractionPayload& Payload);
	FOnQuestInteractionSuccess OnQuestInteractionSuccess;

	void OnInteractionStateAddEvent(UPDWInteractionReceiverComponent* Comp, const FGameplayTag& PrevTag, const FGameplayTag& NewTag);
	FOnInteractionStateAdd OnInteractionStateAdd;

	void OnQuestInteractionStateAddEvent(UPDWInteractionReceiverComponent* Comp, const FGameplayTag& PrevTag, const FGameplayTag& NewTag);
	FOnQuestInteractionStateAdd OnQuestInteractionStateAdd;

	UFUNCTION(BlueprintCallable)
	void OnHintRequestEvent(const FGameplayTag& inMinigameID);
	UPROPERTY(BlueprintAssignable)
	FOnHintRequest OnHintRequest;

	UFUNCTION(BlueprintCallable)
	void OnResetHintTimerRequestEvent();
	UPROPERTY(BlueprintAssignable)
	FOnResetHintTimerRequest OnResetHintTimerRequest;
	UFUNCTION(BlueprintCallable)
	void OnShowHintEvent(USceneComponent* Owner,const FGameplayTag& inMinigameID);
	UPROPERTY(BlueprintAssignable)
	FOnShowHint OnShowHint;

	UFUNCTION(BlueprintCallable)
	void OnTargetDeactivationEvent(AActor* DeactivatingActor);
	UPROPERTY(BlueprintAssignable)
	FOnTargetDeactivation OnTargetDeactivation;


	FOnRemappingButtonAction OnRemappingButtonFocus;
	FOnRemappingButtonAction OnRemappingButtonClick;
	FOnRemappingButtonAction2 OnRemappingButtonNewKeyChosen;
	FOnSimpleEvent OnOverlayOpenAnimationFinished;
	FOnSimpleEvent OnOverlayCloseAnimationFinished;
	FOnRemappingButtonAction OnRemappingButtonInvalidNewKeyChosen;

	UFUNCTION(BlueprintCallable)
	void OnSwapCharacterMenuClosedEvent();
	UPROPERTY(BlueprintAssignable)
	FOnSwapCharacterMenuClosed OnSwapCharacterMenuClosed;

	UFUNCTION(BlueprintCallable)
	void OnGrapplingHookTargetAquiredEvent(const TArray<AActor*>& Targets,APDWPlayerController* ControllerSender);
	UPROPERTY(BlueprintAssignable)
	FOnGrapplingHookTargetAquired OnGrapplingHookTargetAquired;

	UFUNCTION(BlueprintCallable)
	void OnTransitionInEndEvent();
	UPROPERTY(BlueprintAssignable)
	FOnTransitionInEnd OnTransitionInEnd;

	UFUNCTION(BlueprintCallable)
	void OnTransitionOutEndEvent();
	UPROPERTY(BlueprintAssignable)
	FOnTransitionOutEnd OnTransitionOutEnd;

	UFUNCTION()
	void OnPupCustomizeEvent();
	FOnPupCustomize OnPupCustomize;

	UFUNCTION()
	void OnDinoCustomizeEvent();
	FOnDinoCustomize OnDinoCustomize;

	UFUNCTION()
	void OnDinoNeedCompletedEvent();
	FOnDinoNeedCompleted OnDinoNeedCompleted;

	UFUNCTION()
	void OnPupTreatCollectedEvent();
	FOnPupTreatCollected OnPupTreatCollected;

	UFUNCTION()
	void OnQuestCompletedEvent(const FGameplayTag& QuestID);
	FOnQuestCompleted OnQuestCompleted;

	UFUNCTION()
	void OnDinoPenUnlockedEvent(const FGameplayTag& DinoPenID);
	FOnDinoPenUnlocked OnDinoPenUnlocked;	

	FOnSlotOnFocus OnSlotOnFocus;

	UFUNCTION()
	void OnLocationEnterEvent(const FGameplayTag& CurrentAreaId);
	FOnLocationEnter OnLocationEnter;

	UFUNCTION(BlueprintCallable)
	void OnMovingMeshBehaviourEvent(USceneComponent* SceneComponent);
	UPROPERTY(BlueprintAssignable)
	FOnMovingMeshBehaviour OnMovingMeshBehaviour;

protected:
	// #TODO DM: consider using general struct for general event, right now is just for interaction.
	TMap<FGameplayTag, FOnInteractionExecuted> InteractionEventsMap;
	
};
