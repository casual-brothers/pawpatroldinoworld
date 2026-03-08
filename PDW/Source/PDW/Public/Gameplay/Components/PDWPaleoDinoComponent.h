// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWPaleoDinoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDinoHappy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeforeEndPlay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDinoInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinigameEnd, FGameplayTag, MinigameTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivateInteraction, FGameplayTag, InteractionTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchSO, FGameplayTag, NeedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowNeed, FGameplayTag, NeedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowNeedMat, FGameplayTag, NeedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowFakeNeed, FGameplayTag, NeedTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivateMinigame, FGameplayTag, MinigameTag);

class APDWDinoPenArea;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWPaleoDinoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPDWPaleoDinoComponent();

	UFUNCTION()
	void HandleNeed(FGameplayTag NewNeed);

	UFUNCTION()
	void HandleFakeNeed(FGameplayTag FakeNeed);

	UFUNCTION()
	void HandleWaterStationPlayerInteraction();

	UFUNCTION()
	void HandleFoodStationPlayerInteraction();

	UFUNCTION(BlueprintCallable)
	void OnSearchSOCompleted(bool Success);

	UFUNCTION(BlueprintCallable)
	void OnInteractionSuccess();


	UFUNCTION()
	void SetPenAreaRef(APDWDinoPenArea* _PenAreaRef) { PenAreaRef = _PenAreaRef; };

	UFUNCTION()
	void Init(FGameplayTag _PenTag, APDWDinoPenArea* _PenAreaRef, bool _CanMove);

	UFUNCTION(BlueprintPure)
	APDWDinoPenArea* GetPenAreaRef() { return PenAreaRef; };

	UFUNCTION()
	void SetCanShowNeeds(bool _CanShowNeeds);

	UFUNCTION(BlueprintPure)
	bool GetCanShowNeeds() { return CanShowNeeds; };

	UFUNCTION(BlueprintPure)
	bool GetNeedPlayerHelp() { return NeedPlayerHelp; };

	UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentNeed();

		UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentFakeNeed();

	UFUNCTION(BlueprintPure)
	FGameplayTag GetPenTag() { return PenTag; };

	UFUNCTION()
	void HandleMinigameEnd(FGameplayTag MinigameTag);

	UFUNCTION()
	void SetPenTag(FGameplayTag _PenTag) { PenTag = _PenTag; };

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintAssignable)
	FOnDinoHappy OnDinoHappy;

	UPROPERTY(BlueprintAssignable)
	FOnActivateInteraction OnActivateInteraction;

	UPROPERTY(BlueprintAssignable)
	FOnActivateInteraction OnDeactivateInteraction;

	UPROPERTY(BlueprintAssignable)
	FOnActivateMinigame OnActivateMinigame;

	UPROPERTY(BlueprintAssignable)
	FOnSearchSO OnSearchSO;

	UPROPERTY(BlueprintAssignable)
	FOnShowNeed OnShowNeed;

	UPROPERTY(BlueprintAssignable)
	FOnShowNeedMat OnShowNeedMat;

	UPROPERTY(BlueprintAssignable)
	FOnShowFakeNeed OnShowFakeNeed;

	UPROPERTY(BlueprintAssignable)
	FBeforeEndPlay BeforeEndPlay;

	UPROPERTY(BlueprintAssignable)
	FOnDinoInit OnInit;

	UPROPERTY(BlueprintAssignable)
	FOnMinigameEnd OnMinigameEnd;

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void InitCustomization();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag PenTag = {};

	UPROPERTY()
	APDWDinoPenArea* PenAreaRef;

	UPROPERTY()
	bool NeedPlayerHelp = false;

	UPROPERTY()
	bool CanShowNeeds = false;

	UPROPERTY()
	bool CanMove = true;

};
