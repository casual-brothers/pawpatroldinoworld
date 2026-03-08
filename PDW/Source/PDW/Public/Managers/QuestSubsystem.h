// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Data/PDWGameplayStructures.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "QuestSubsystem.generated.h"

class UPDWInteractionReceiverComponent;
class UFlowComponent;
class UPDWDialogueFlowAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMessageSignature, UTexture2D*, Icon, FText, CharName, FText, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlockingDialogueSignature);
/**
 * 
 */
UCLASS()
class PDW_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	static UQuestSubsystem* Get(UObject* WorldContext);

	void QuestStart(UBaseFlowQuest* Quest, bool FromLoad = false);

	void EndQuest(const FGameplayTag& QuestID);

	void StartStep(const FPDWQuestStepData& StepData);

	UFUNCTION(BlueprintCallable)
	void SetCurrentQuestDescription(FPDWStepDescriptionData DescriptionData);

	UFUNCTION(BlueprintCallable)
	FPDWQuestStepData GetCurrentQuestStepData();

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentQuestID();

	UFUNCTION(BlueprintCallable)
	FText GetCurrentQuestName();

	UFUNCTION(BlueprintCallable)
	TArray<FText> GetCurrentQuestDescriptionHistory();

	void LoadStep();

	UFUNCTION(BlueprintCallable)
	void ClearQuests();

	UFUNCTION()
	UBaseFlowQuest* GetCurrentQuest();

	UFUNCTION()
	TMap<FGameplayTag, UBaseFlowQuest*> GetActiveQuests() const {return ActiveQuests;};
	
	UFUNCTION()
	void SetQuestTargets(FPDWQuestTargetData TargetsData);

	UFUNCTION()
	void OnQuestTargetLoaded(FGameplayTagContainer Target, AActor* ActorRef, FPDWQuestTargetData& TargetData, bool IsLoaded);

	//UFUNCTION()
	//void RemoveQuestTarget(FGameplayTag Target);

	UFUNCTION()
	void RemoveQuestTarget(AActor* Target, FPDWQuestTargetData& TargetData);

	UFUNCTION()
	void ClearBreadcrumbs();

	void Deinitialize() override;

	UPDWDialogueFlowAsset* GetCurrentQuestDialogueFlow();

	void SetCurrentQuestDialogueFlow(UPDWDialogueFlowAsset* inFlow);

	UFUNCTION()
	FString GetQuestIndexByTag(FGameplayTag QuestTag);

public:

	UPROPERTY(BlueprintAssignable)
	FOnQuestTextUpdateSignature OnQuestDescriptionUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnQuestTargetUpdateSignature OnQuestTargetUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnQuestStepStartSignature OnQuestStepStartSignature;

	UPROPERTY(BlueprintAssignable)
	FOnQuestUpdateSignature OnQuestStarted;

	UPROPERTY(BlueprintAssignable)
	FOnQuestUpdateSignature OnQuestEnded;

	UPROPERTY(BlueprintAssignable)
	FMessageSignature OnQuestMessage;

	UPROPERTY(BlueprintAssignable)
	FMessageSignature OnBlockingQuestMessage;

	UPROPERTY(BlueprintAssignable)
	FBlockingDialogueSignature OnStartBlockingDialogue;

	UPROPERTY(BlueprintAssignable)
	FBlockingDialogueSignature OnEndBlockingDialogue;

	UFUNCTION()
	void InitGameplay();

	UFUNCTION()
	FPDWWaypointData GetWaypointsByArea(FGameplayTag AreaTag);

	UFUNCTION()
	void AddWaypoint(APDW_NavigationWaypoint* SpawnedWaypoint);


protected:

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, UBaseFlowQuest*> ActiveQuests = {};
	
	UPROPERTY()
	TMap<FGameplayTag, FPDWWaypointData> WaypointsData = {};

	UFUNCTION()
	void OnObservedActorLoaded(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator);

	UFUNCTION()
	void OnObservedActorUnloaded(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator);

	UFUNCTION()
	void OnMultiplayerStateChange();

private:
	UPROPERTY()
	UPDWQuestConfig* QuestConfig;
	
	UPROPERTY()
	UPDWDialogueFlowAsset* CurrentDialogFlowAsset = nullptr;
};
