// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/PDWGameplayStructures.h"
#include "Gameplay/Components/PDWPaleoDinoComponent.h"
#include "PDWDinoPen.generated.h"

class UFlowComponent;
class APDWFoodStation;
class APDWDinoNest;
class APDWPaleoCenterDino;
class APDWDinoPenArea;
class UPDWInteractionReceiverComponent;
class UPDWMinigameConfigComponent;
class UPDWDinoNestHandler;
class APDWPaleoCenterCustomization;

UCLASS(BlueprintType, Blueprintable)
class PDW_API UPDWDinoPen : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPDWDinoPen();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	float SimulatedWaterDecrement = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 MaxCustomizations = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 MaxEggs = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag DinoPenTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag DinoTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag DinoNestTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag FoodStationTag = {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag WaterStationTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag PenAreaTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag EggTypeTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag FoodTypeTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag FoodPlantTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag SeedTypeTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag CustomizationTypeTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag FoodFlowTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag EggsFlowTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag BabyDinoTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTagContainer WaterHoseMinigameTag = {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTagContainer MedicalKitMinigameTag = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FText DinoDisplayName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TSoftObjectPtr<UTexture2D> DinoDisplayIcon{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FGameplayTag NestTeleportTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TObjectPtr<UDataLayerAsset> MainDL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TObjectPtr<UDataLayerAsset> BabyDinoDL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TObjectPtr<UDataLayerAsset> FoodSourcesDL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TMap<FGameplayTag, TObjectPtr<UDataLayerAsset>> MinigamesDL;

	UFUNCTION()
	void OnStartPlay();

	UFUNCTION()
	void StartPaleoQuests(bool FromLoad);

	UFUNCTION()
	void Uninit();

	UFUNCTION()
	void BindFlowCompEvents();

	UFUNCTION()
	void UnbindFlowCompEvents();

	UFUNCTION()
	void UpdateDinoNeeds(FGameplayTag NewNeed);

	UFUNCTION()
	float DecrementFoodStation(float RemovedFood);

	UFUNCTION()
	float DecrementWaterStation(float RemovedFood);

	UFUNCTION()
	bool GetShouldSimulate();

	UFUNCTION()
	FGameplayTag GetCurrentDinoNeed();

	UFUNCTION()
	FGameplayTag GetCurrentFakeDinoNeed();

	UFUNCTION()
	bool GetIsPenUnlocked();

	UFUNCTION()
	bool GetIsPenActive();

	UFUNCTION()
	void SetIsPenActive(bool _IsActive, bool ResetNeedTimer = false);

	UFUNCTION()
	FPDWDinoPenInfo GetPenInfo();

	UFUNCTION()
	void UnlockPen(bool StartInactive = false);

	UFUNCTION()
	AActor* GetDinoActorRef() { return DinoCompRef ? DinoCompRef->GetOwner() : nullptr; };

	UFUNCTION()
	APDWDinoPenArea* GetPenAreaRef() { return PenAreaRef; };

	UFUNCTION()
	void HandleFakeNeed(FGameplayTag FakeNeed);

protected:

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentFoodSources = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWaterSources = 0;

	UPROPERTY()
	UPDWPaleoDinoComponent* DinoCompRef = {};

	UPROPERTY()
	UPDWDinoNestHandler* NestRefHandler = {};

	UPROPERTY()
	AActor* NestRef = {};

	UPROPERTY()
	AActor* FoodStationRef = {};

	UPROPERTY()
	AActor* WaterStationRef = {};

	UPROPERTY()
	APDWDinoPenArea* PenAreaRef = {};

	UPROPERTY()
	TArray<APDWPaleoCenterCustomization*> CustomizationsRef = {};

	UPROPERTY()
	UFlowComponent* EggsFlowCompRef = {};

	UPROPERTY()
	UFlowComponent* FoodFlowCompRef = {};

	UFUNCTION()
	void OnItemCollected(FGameplayTag ItemTag);

	UFUNCTION()
	void OnDinoPenAreaEnter(AActor* OverlappingActor);

	UFUNCTION()
	void OnDinoPenAreaExit(AActor* OverlappingActor);

	UFUNCTION()
	void OnFoodStationMinigameSuccess(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnWaterStationInteractionSuccess(const FPDWInteractionPayload& Payload);

	UFUNCTION()
	void OnNestMinigameSuccess(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnWaterHoseMinigameSuccess(const FMiniGameEventSignature& inSignature);
	
	UFUNCTION()
	void OnMedicalKitMinigameSuccess(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnNestFull(AActor* Source);

	UFUNCTION()
	void OnDinoHappy();

	UFUNCTION()
	void StartFoodQuest(bool Load = false);

	UFUNCTION()
	void StartEggsQuest(bool Load = false);

	UFUNCTION()
	void OnFlowCompRegistered(UFlowComponent* Component);

	UFUNCTION()
	void OnFlowCompUnregistered(UFlowComponent* Component);

	UFUNCTION()
	void CheckWaterStation();

	UFUNCTION()
	void CheckFoodStation();

	UFUNCTION()
	void OnMinigameActivation(FGameplayTag DinoNeed);

};
