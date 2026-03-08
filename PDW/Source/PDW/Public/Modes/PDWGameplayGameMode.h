// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GameModes/NebulaFlowBaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWGameplayGameMode.generated.h"

class UFlowComponent;
class UCLDebugWidget;
class UGameplayHUD;
class APDWPlayerController;
class APDWCharacter;
class UGameStateFlowAsset;
class UPDWFsmHelper;
class UInputAction;
class UPDWMinigameConfigComponent;
class ULevelSequencePlayer;
class APDWEnvironmentSkyArea;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSequenceSignatureDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameplayStateEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameReady);
/**
 *
 */
UCLASS()
class PDW_API APDWGameplayGameMode : public ANebulaFlowBaseGameMode
{
	GENERATED_BODY()

public:

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void StartPlay() override;

	UFUNCTION(BlueprintPure, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static APDWGameplayGameMode* Get(const UObject* WorldContextObject)
	{
		APDWGameplayGameMode* GM = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
		//check(GM);
		return GM;
	}

	UFUNCTION(BlueprintPure)
	APDWPlayerController* GetPlayerControllerOne() const { return PC_One; }
	UFUNCTION(BlueprintPure)
	APDWPlayerController* GetPlayerControllerTwo() const { return PC_Two; }
		
	void RestartPlayer(AController* NewPlayer) override;
	
	UFUNCTION(BlueprintCallable)
	void CreatePlayer2(int32 UserIndex);
	
	UFUNCTION(BlueprintCallable)
	void RemovePlayer2();

	UFUNCTION(BlueprintCallable)
	bool IsGameReady() const {return bIsGameReady;};

	UFUNCTION(BlueprintCallable)
	void ReplayMinigame(FGameplayTagContainer MinigameTags);

	UPROPERTY(BlueprintAssignable)
	FSequenceSignatureDelegate OnLevelSequenceStart;
	UPROPERTY(BlueprintAssignable)
	FSequenceSignatureDelegate OnLevelSequenceEnd;

	UPROPERTY(BlueprintAssignable)
	FGameReady GameReady;

	UPROPERTY()
	bool SwitcherWaitForAction = false;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UPDWFsmHelper> FSMHelperClass{};

	UPROPERTY()
	UPDWFsmHelper* FSMHelper;
	
	UPROPERTY(Transient)
	TSet<FName> DisabledInputActions = {};

	UPROPERTY()
	UPDWMinigameConfigComponent* CurrentMinigame = {};

	UPROPERTY(BlueprintReadOnly)
	ULevelSequencePlayer* CurrentLevelSequencePlayer = {};
	
	UPROPERTY()
	UCLDebugWidget* ClPage;

	UPROPERTY()
	TArray<APDWEnvironmentSkyArea*> SkyAreas = {};

protected:

	void PreInitializeComponents() override;

	UPROPERTY()
	TObjectPtr<APDWPlayerController> PC_One;
	UPROPERTY()
	TObjectPtr<APDWPlayerController> PC_Two;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCLDebugWidget> CLWidgetClass;

	UPROPERTY()
	bool InitStateReady = false;

	UPROPERTY()
	bool StartPlayReady = false;

	UPROPERTY()
	bool IsTestMap = false;

	UPROPERTY()
	bool bIsGameReady = false;

	UPROPERTY()
	FTransform BeforeReplayMinigameLocation = FTransform();

	UFUNCTION(exec)
	void TriggerFlowEntryPoint(const FName& EventName);

	UFUNCTION()
	void OnTeleportCompletedEvent();

	UFUNCTION()
	void OnGameReady();

	UFUNCTION()
	void OnInitGameStateEnter();

	UFUNCTION()
	void OnSwitcherStateEnter();

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;		

	UPROPERTY()
	FPDWQuestTargetLocs PendingReplayMinigame = {};

	UFUNCTION()
	void OnMinigameTeleportCompleted();
		
	UFUNCTION()
	void OnComponentRegistered(UFlowComponent* Component);

	UFUNCTION()
	void ProcessInteraction(UPDWInteractionReceiverComponent* InteractionComp);


	UFUNCTION()
	void OnReplayMinigameComplete(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnReplayMinigameLeft(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnBeforeSave();

	UFUNCTION()
	void ReturnToRyder();

	FDelegateHandle BeforeSaveHandle;
};
