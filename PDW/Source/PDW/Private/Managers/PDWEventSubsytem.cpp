// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "FlowComponent.h"
#include "Nodes/FlowNode.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

UPDWEventSubsytem* UPDWEventSubsytem::Get(UObject* WorldContext)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	return GameInstance ? GameInstance->GetSubsystem<UPDWEventSubsytem>() : nullptr;
}

void UPDWEventSubsytem::OnGameplayStateEnterEvent()
{
	OnGameplayStateEnter.Broadcast();
}

void UPDWEventSubsytem::OnGameplayStateExitEvent()
{
	OnGameplayStateExit.Broadcast();
}

void UPDWEventSubsytem::OnSwitcherStateWaitForActionEvent()
{
	OnSwitcherStateWaitForAction.Broadcast();
}

void UPDWEventSubsytem::OnCutsceneStartEvent()
{
	OnCutsceneStart.Broadcast();
}

void UPDWEventSubsytem::OnCutsceneEndEvent()
{
	OnCutsceneEnd.Broadcast();
}

void UPDWEventSubsytem::OnItemCollectedEvent(FGameplayTag ItemTag)
{
	OnItemCollected.Broadcast(ItemTag);
}

void UPDWEventSubsytem::OnEggCollectedEvent(FGameplayTag EggTag)
{
	OnEggCollected.Broadcast(EggTag);
}

void UPDWEventSubsytem::OnFoodCollectedEvent(FGameplayTag FoodTag)
{
	OnFoodCollected.Broadcast(FoodTag);
}

void UPDWEventSubsytem::OnSeedCollectedEvent(FGameplayTag SeedTag)
{
	OnSeedCollected.Broadcast(SeedTag);
}

void UPDWEventSubsytem::OnDinoPenCosmeticCollectedEvent(FGameplayTag CosmeticTag)
{
	OnDinoPenCosmeticCollected.Broadcast(CosmeticTag);
}

void UPDWEventSubsytem::OnEggHatchEvent(FGameplayTag NestTag)
{
	OnEggHatch.Broadcast(NestTag);
}

void UPDWEventSubsytem::OnInteractionCompleteEvent(UPDWInteractionReceiverComponent* Interaction)
{
	OnInteractionComplete.Broadcast(Interaction);
}

void UPDWEventSubsytem::OnObservedActorLoadedEvent(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator)
{
	OnObservedActorLoaded.Broadcast(Actor, Component, TargetData, Instigator);
}

void UPDWEventSubsytem::OnObservedActorUnloadedEvent(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator)
{
	OnObservedActorUnloaded.Broadcast(Actor, Component, TargetData, Instigator);
}	

void UPDWEventSubsytem::OnMultiplayerStateChangeEvent()
{
	OnMultiplayerStateChange.Broadcast();
}

void UPDWEventSubsytem::OnPlayerAreaChangeEvent(const APDWPlayerController* Player, FGameplayTagContainer AllowedPups, bool EnterArea, const APDWAutoSwapArea* NewArea)
{
	OnPlayerAreaChange.Broadcast(Player, AllowedPups, EnterArea, NewArea);
}

void UPDWEventSubsytem::OnPenAreaBeginOverlapEvent(FPDWDinoPenInfo PenInfo, AActor* OverlappedActor)
{
	OnPenAreaBeginOverlap.Broadcast(PenInfo, OverlappedActor);
}

void UPDWEventSubsytem::OnPenAreaEndOverlapEvent(AActor* OverlappedActor)
{
	OnPenAreaEndOverlap.Broadcast(OverlappedActor);
}

void UPDWEventSubsytem::OnDinoNeedUpdateEvent(FPDWDinoPenNeed NewNeed)
{
	OnDinoNeedUpdate.Broadcast(NewNeed);
}

FOnInteractionExecuted* UPDWEventSubsytem::BindToInteractionEvents(const FGameplayTag& inEventID)
{
	if (!InteractionEventsMap.Contains(inEventID))
	{
		FOnInteractionExecuted Delegate;
		InteractionEventsMap.Add(inEventID,Delegate);
	}
	return &InteractionEventsMap[inEventID];
}

void UPDWEventSubsytem::TriggerInteractionEvent(const FGameplayTag& inEventID, const FInteractionEventSignature& inSignature)
{
	if (InteractionEventsMap.Contains(inEventID))
	{
		InteractionEventsMap[inEventID].Broadcast(inSignature);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPDWEventSubsytem::TriggerInteractionEvent - No delegates found for event %s"), *inEventID.ToString());
	}
}

void UPDWEventSubsytem::OnPuzzlePossesEvent(bool bPosses, USceneComponent* PuzzlePiece)
{
	OnPuzzlePosses.Broadcast(bPosses,PuzzlePiece);
}

void UPDWEventSubsytem::OnPageReadyEvent()
{
	OnPageReady.Broadcast();
}

void UPDWEventSubsytem::OnJoinRequestRefusedEvent()
{
	OnJoinRequestRefused.Broadcast();
}

void UPDWEventSubsytem::OnRockRemovedEvent(AActor* inRockRemoved)
{
	OnRockRemoved.Broadcast(inRockRemoved);
}

void UPDWEventSubsytem::OnMinigameCompleteOrLeftEvent(AActor* Actor)
{
	OnMinigameCompleteOrLeft.Broadcast(Actor);
}

void UPDWEventSubsytem::OnMinigameInitializationEvent()
{
	OnMinigameInitialization.Broadcast();
}

void UPDWEventSubsytem::OnJumpOnSpline(const float& inJumpValue)
{
	OnJumpEventOnSpline.Broadcast(inJumpValue);
}

void UPDWEventSubsytem::NotifyBindEvent(const FGameplayTag& ActionToBind, APDWPlayerController* Owner)
{
	OnNotifyBind.Broadcast(ActionToBind,Owner);
}

void UPDWEventSubsytem::NotifyUnBindEvent(int32 BindingHandle, APDWPlayerController* Owner)
{
	OnNotifyUnBind.Broadcast(BindingHandle,Owner);
}

void UPDWEventSubsytem::OnRamModeEvent()
{
	OnRamMode.Broadcast();
}

void UPDWEventSubsytem::OnStopRamEvent()
{
	OnStopRamMode.Broadcast();
}

void UPDWEventSubsytem::OnStopJumpOnSpline()
{
	OnStopJumpEvent.Broadcast();
}

void UPDWEventSubsytem::OnQuestTalkingEvent()
{
	OnQuestTalking.Broadcast();
}

void UPDWEventSubsytem::OnInitGameEnterEvent()
{
	OnInitGameEnter.Broadcast();
}

void UPDWEventSubsytem::OnSettingsAppliedEvent()
{
	OnSettingsApplied.Broadcast();
}

void UPDWEventSubsytem::OnGameOptionsChangedEvent(EGameOptionsId SettingsId, int32 CurrentValueIndex)
{
	OnGameOptionsChanged.Broadcast(SettingsId, CurrentValueIndex);
}

void UPDWEventSubsytem::OnModalResponseEvent(const FString Response)
{
	OnModalResponse.Broadcast(Response);
}

void UPDWEventSubsytem::OnDinoPenActivityChangeEvent(const FGameplayTag PenTag, bool IsActive)
{
	OnDinoPenActivityChange.Broadcast(PenTag, IsActive);
}

void UPDWEventSubsytem::OnInteractionSuccessEvent(const FPDWInteractionPayload& Payload)
{
	OnInteractionSuccess.Broadcast(Payload);
}

void UPDWEventSubsytem::OnInteractionStateAddEvent(UPDWInteractionReceiverComponent* Comp, const FGameplayTag& PrevTag, const FGameplayTag& NewTag)
{
	OnInteractionStateAdd.Broadcast(Comp, PrevTag, NewTag);
}

void UPDWEventSubsytem::OnQuestInteractionSuccessEvent(const FPDWInteractionPayload& Payload)
{
	OnQuestInteractionSuccess.Broadcast(Payload);
}

void UPDWEventSubsytem::OnQuestInteractionStateAddEvent(UPDWInteractionReceiverComponent* Comp, const FGameplayTag& PrevTag, const FGameplayTag& NewTag)
{
	OnQuestInteractionStateAdd.Broadcast(Comp, PrevTag, NewTag);
}

void UPDWEventSubsytem::OnHintRequestEvent(const FGameplayTag& inMinigameID)
{
	OnHintRequest.Broadcast(inMinigameID);
}

void UPDWEventSubsytem::OnResetHintTimerRequestEvent()
{
	OnResetHintTimerRequest.Broadcast();
}

void UPDWEventSubsytem::OnShowHintEvent(USceneComponent* Owner,const FGameplayTag& inMinigameID)
{
	OnShowHint.Broadcast(Owner,inMinigameID);
}

void UPDWEventSubsytem::OnTargetDeactivationEvent(AActor* DeactivatingActor)
{
	OnTargetDeactivation.Broadcast(DeactivatingActor);
}

void UPDWEventSubsytem::OnSwapCharacterMenuClosedEvent()
{
	OnSwapCharacterMenuClosed.Broadcast();
}

void UPDWEventSubsytem::OnGrapplingHookTargetAquiredEvent(const TArray<AActor*>& Targets, APDWPlayerController* ControllerSender)
{
	OnGrapplingHookTargetAquired.Broadcast(Targets,ControllerSender);
}

void UPDWEventSubsytem::OnTransitionInEndEvent()
{
	OnTransitionInEnd.Broadcast();
}

void UPDWEventSubsytem::OnTransitionOutEndEvent()
{
	OnTransitionOutEnd.Broadcast();
}

void UPDWEventSubsytem::OnPupCustomizeEvent()
{
	OnPupCustomize.Broadcast();
}

void UPDWEventSubsytem::OnDinoCustomizeEvent()
{
	OnDinoCustomize.Broadcast();
}

void UPDWEventSubsytem::OnDinoNeedCompletedEvent()
{
	OnDinoNeedCompleted.Broadcast();
}

void UPDWEventSubsytem::OnPupTreatCollectedEvent()
{
	OnPupTreatCollected.Broadcast();
}

void UPDWEventSubsytem::OnQuestCompletedEvent(const FGameplayTag& QuestID)
{
	OnQuestCompleted.Broadcast(QuestID);
}

void UPDWEventSubsytem::OnDinoPenUnlockedEvent(const FGameplayTag& DinoPenID)
{
	OnDinoPenUnlocked.Broadcast(DinoPenID);
}

void UPDWEventSubsytem::OnLocationEnterEvent(const FGameplayTag& CurrentAreaId)
{
	OnLocationEnter.Broadcast(CurrentAreaId);
}

void UPDWEventSubsytem::OnMovingMeshBehaviourEvent(USceneComponent* SceneComponent)
{
	OnMovingMeshBehaviour.Broadcast(SceneComponent);
}