// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PDWBabyPaleoDinoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBabyDinoInit);

class APDWDinoPenArea;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWBabyPaleoDinoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWBabyPaleoDinoComponent();

	UFUNCTION()
	void Init(FGameplayTag _PenTag, APDWDinoPenArea* _PenArea);

	UFUNCTION(BlueprintPure)
	FGameplayTag GetPenTag() { return PenTag; };

	UFUNCTION(BlueprintPure)
	APDWDinoPenArea* GetPenAreaRef() { return PenArea; };
	
	UPROPERTY(BlueprintAssignable)
	FOnBabyDinoInit OnInit;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	FGameplayTag PenTag = {};

	UPROPERTY()
	APDWDinoPenArea* PenArea = {};

		
};
