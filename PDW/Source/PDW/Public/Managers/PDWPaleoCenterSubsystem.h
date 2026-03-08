// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWPaleoCenterSubsystem.generated.h"

class UDataLayerAsset;
class UPDWDinoPen;
class UPDWPaleoCenterSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDinoLoad, AActor*, DinoRef);

UCLASS()
class PDW_API UPDWPaleoCenterSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION()
	void Init();

	UFUNCTION()
	void OnStartPlay();

	UFUNCTION()
	void StartPaleoQuests(bool FromLoad);

	UFUNCTION()
	void Uninit();

	UFUNCTION(BlueprintCallable)
	void UnlockDinoPen(FGameplayTag DinoPenTag,	bool StartInactive = false);

	UFUNCTION(BlueprintCallable)
	void ChangeDinoPenActivity(FGameplayTag DinoPenTag, bool IsActive, bool ResetNeedTimer = false);

	UFUNCTION(BlueprintCallable)
	bool GetDinoPenActivity(FGameplayTag DinoPenTag);

	UFUNCTION(BlueprintCallable)
	bool IsDinoPenUnlocked(FGameplayTag DinoPenTag);

	UFUNCTION(BlueprintCallable)
	AActor* GetDinoActorRef(FGameplayTag DinoPenTag);

	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetTickControl(bool IsActive){TickControl = IsActive; };

	UFUNCTION(BlueprintCallable)
	bool GetTickControl(){return TickControl; };

	UFUNCTION()
	void UpdateFoodAndWaterQuantity(float DeltaTime);

	UFUNCTION()
	void CheckDinoNeeds(float DeltaTime);

	UFUNCTION()
	void AddDinoPenRef(UPDWDinoPen* DinoPen);

	UFUNCTION()
	void RemoveDinoPenRef(UPDWDinoPen* DinoPen);

	UFUNCTION()
	void ResetDinoNeedTimer() { CurrentUpdateDinoNeedsTimer = 0.f; };

	UFUNCTION()
	TArray<UPDWDinoPen*> GetDinoPens() { return DinoPens; };

	UFUNCTION(BlueprintPure)
	FPDWDinoPenInfo GetDinoPenInfo(FGameplayTag DinoPenTag);

	UFUNCTION(BlueprintPure)
	FPDWDinoPenInfo GetDinoPenInfoAndActivity(FGameplayTag DinoPenTag);

	virtual TStatId GetStatId() const override
	{
		return GetStatID();
	}
	
	UFUNCTION(BlueprintCallable)
	void SetLastItemNestInfo(const FGameplayTag& NewTag) {LastItemOfANest = NewTag;};

	UFUNCTION(BlueprintPure)
	const FGameplayTag& GetLastItemNestInfo() {return LastItemOfANest;};

	UFUNCTION(BlueprintCallable)
	APDWDinoPenArea* GetPenAreaByPenTag(FGameplayTag PenTag);

	UFUNCTION()
	void OnDinoLoadedEvent(AActor* DinoRef);

	UPROPERTY(BlueprintAssignable)
	FOnDinoLoad OnDinoLoad;

protected:


	UPROPERTY()
	UPDWPaleoCenterConfig* PaleoCenterConfig;

	UPROPERTY()
	bool TickControl = true;

	UPROPERTY(EditInstanceOnly)
	TArray<UPDWDinoPen*> DinoPens;

	UPROPERTY()
	FGameplayTag UnlockingDinoPen = {};

	UPROPERTY()
	float CurrentUpdateDinoNeedsTimer = 0.f;

	UPROPERTY()
	float CurrentUpdateFoodAndWaterTimer = 0.f;

	UPROPERTY()
	TArray<FGameplayTag> AllDinoNeeds = {};
	
	UPROPERTY()
	FGameplayTag LastItemOfANest;
};
