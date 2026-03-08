// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PDWWaterStationHandler.generated.h"

class APDWPaleoCenterDino;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSmartObjectUpdate, bool, IsActive);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWWaterStationHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWWaterStationHandler();
		
	UFUNCTION(BlueprintCallable)
	void InitWaterStation(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentWater();

	UFUNCTION(BlueprintCallable)
	void SetCurrentWater(float NewValue);

	UFUNCTION()
	void HandleEndInteraction();

	UPROPERTY(BlueprintAssignable)
	FOnInit OnInit;

	UPROPERTY(BlueprintAssignable)
	FOnSmartObjectUpdate OnSmartObjectUpdate;

private:

	UPROPERTY()
	FGameplayTag DinoPenTag = FGameplayTag();
};
