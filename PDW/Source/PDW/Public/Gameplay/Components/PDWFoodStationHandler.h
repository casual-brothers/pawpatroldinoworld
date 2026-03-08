// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PDWFoodStationHandler.generated.h"

class APDWPaleoCenterDino;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoodStationInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoodCollect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoodStationSOUpdate, bool, IsActive);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWFoodStationHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWFoodStationHandler();
		
	UFUNCTION(BlueprintCallable)
	void InitFoodStation(FGameplayTag _DinoPenTag, FGameplayTag _FoodTypeTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentStationFood();

	UFUNCTION(BlueprintCallable)
	void SetCurrentStationFood(float NewValue);

	UFUNCTION()
	void HandleMinigameEnd();

	UPROPERTY(BlueprintAssignable)
	FOnFoodStationInit OnInit;

	UPROPERTY(BlueprintAssignable)
	FOnFoodCollect OnFoodCollect;

	UPROPERTY(BlueprintAssignable)
	FOnFoodStationSOUpdate OnSmartObjectUpdate;

private:

	UPROPERTY()
	FGameplayTag DinoPenTag = FGameplayTag();

	UPROPERTY()
	FGameplayTag FoodTypeTag = FGameplayTag();

	UPROPERTY()
	int32 MaxFoodInStation = 0;
};
