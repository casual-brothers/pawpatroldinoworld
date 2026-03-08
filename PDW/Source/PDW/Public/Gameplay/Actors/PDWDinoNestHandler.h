// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PDWDinoNestHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNestFull, AActor*, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNestEggCollected, int32, TotalEggs);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PDW_API UPDWDinoNestHandler : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPDWDinoNestHandler();

	UFUNCTION()
	void OnEggCollected(FGameplayTag EggCollected);

	UFUNCTION(BlueprintCallable)
	int32 GetCollectedEgss(){ return CollectedEgss; };

	UFUNCTION()
	int32 GetMaxEgss(){ return EggsToCollect; };
	
	UFUNCTION()
	void SetMaxEgss(int32 _EggsToCollect){ EggsToCollect = _EggsToCollect; };

	UFUNCTION()
	void UpdateEggs();

	UFUNCTION()
	void HideEggs();

	UPROPERTY(BlueprintAssignable)
	FOnNestFull OnNestFull;

	UPROPERTY(BlueprintAssignable)
	FOnNestEggCollected OnNestEggCollected;

protected:

	UPROPERTY()
	int32 EggsToCollect = 10;

	UPROPERTY(EditAnywhere)
	FGameplayTag EggTag = {};

	UPROPERTY()
	int32 CollectedEgss = 0;

	UPROPERTY()
	TArray<UActorComponent*> EggsComp = {};
};
