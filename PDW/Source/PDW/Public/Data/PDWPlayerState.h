// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"
#include "PDWGameplayStructures.h"

#include "PDWPlayerState.generated.h"

class APDWVehiclePawn;
class APDWCharacter;
class APDWPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPupTreatChangedDelegate, APDWPlayerState*, State, int,  NewValue, int, Delta);

/**
 * 
 */
UCLASS()
class PDW_API APDWPlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void SetIsOnVehicle(bool bValue);
	
	UFUNCTION(BlueprintPure)
	bool GetIsOnVehicle() const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentPup(const FGameplayTag& inPupTag);

	UFUNCTION(BlueprintCallable)
	const FGameplayTag& GetCurrentPup();

	UFUNCTION(BlueprintPure)
	APawn* GetActivePawn() const;

	UFUNCTION(BlueprintCallable)
	void ChangePupTreats(const int32 Difference);

	UFUNCTION(BlueprintCallable)
	void SetPlayerStates(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void AddPlayerStates(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerStates(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void SetMiniGameVehiclePhysicsOff(const bool bInActive);

	UFUNCTION(BlueprintCallable)
	bool GetMiniGameVehiclePhysicsOff();

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetPlayerStates() const;

	UFUNCTION(Blueprintpure)
	int32 GetExpToLevelUp();
	
	UFUNCTION(Blueprintpure)
	int32 GetCurrentExp();

	UPROPERTY(BlueprintAssignable)
	FOnPupTreatChangedDelegate OnPupTreatChanged;

	APDWCharacter* GetCharacterPup();

	void SetCharacterPup(APDWCharacter* inChar);
	#pragma region TO REMOVE - INVENTORY

	//#TODO : REMOVE ONCE WE HAVE INVENTORY SYSTEM

	UPROPERTY()
	TMap<FName,AActor*> Inventory;

	UFUNCTION(BlueprintCallable)
	AActor* GetItem(const FName& inID);

	UFUNCTION(BlueprintCallable)
	bool CheckItem(const FName& inID) { return Inventory.Contains(inID); };

	UFUNCTION(BlueprintCallable)
	void AddItem(const FName& inID,AActor* inItem);

	UFUNCTION()
	UPDWLevelUpRewardsData* GetRewardData() {return RewardData; };
	#pragma endregion

protected:
	
	UFUNCTION()
	void HandleLevelUp(int32 Level);

	void BeginPlay() override;

	UPROPERTY()
	FGameplayTagContainer PlayerStateContainer;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag CurrentPupTag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APDWCharacter> Character;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APDWVehiclePawn> VehiclePawn;

	bool bIsOnVehicle = false;

	UPROPERTY()
	UPDWLevelUpRewardsData* RewardData = {};

private:

	bool PhysicsOff = false;

	UPROPERTY()
	APDWPlayerController* PDWController = nullptr;
};

	//UFUNCTION()
	//void OnExpGained(int32 ExpGained);


	//UFUNCTION()
	//void OnPostUnlockedLastItem();
