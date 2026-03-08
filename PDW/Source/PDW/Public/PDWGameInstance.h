// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Data/GameOptionsEnums.h"
#include "PDWGameInstance.generated.h"

class UGameStateFlowAsset;

USTRUCT(BlueprintType)
struct FTypeStructNew
{
	GENERATED_USTRUCT_BODY()

public:
	
};

template<typename T>
struct FTypeStruct : public FTypeStructNew
{
	
public:

	T value;
	
};

/**
 *
 */
UCLASS()
class PDW_API UPDWGameInstance : public UNebulaFlowGameInstance
{
	GENERATED_BODY()
public:

	static TObjectPtr<UPDWGameInstance> Get(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void LoadGame();
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void SetMultiPlayerOn(const bool bIsMultiPlayer);
	UFUNCTION(BlueprintCallable)
	bool GetIsMultiPlayerOn() const;


	void Init() override;

	UFUNCTION()
	void SetCurrentSlot(const int SlotNumber);

	UFUNCTION()
	int GetCurrentSlotToLoad() const;
		
	void HandleAppReactivated();

	UFUNCTION()
	void HandleOnControllerPairingChanged();

	UFUNCTION()
	void OnGameReady();

	void InitActivityManager();

protected:

	int CurrentSaveSlotToLoad = -1;

	bool bIsMultiPlayerOn = false;

	void HideLoadingScreen(UWorld* InLoadedWorld) override;

	UFUNCTION()
	void OnRemapJoyConClosedEventCalled(bool bSingleJoyCon);

};
