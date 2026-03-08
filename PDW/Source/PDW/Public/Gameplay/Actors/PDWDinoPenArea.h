// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWDinoPenArea.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPenAreaBeginOverlap, AActor*, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPenAreaEndOverlap, AActor*, Source);

UCLASS()
class PDW_API APDWDinoPenArea : public ATriggerVolume
{
	GENERATED_BODY()
	
public:

	APDWDinoPenArea();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void NotifyActorEndOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintPure)
	FPDWPenAreaBounds GetPenAreaBounds(){ return PenAreaBounds; };

	UPROPERTY(BlueprintAssignable)
	FPenAreaBeginOverlap OnPenAreaBeginOverlap;

	UPROPERTY(BlueprintAssignable)
	FPenAreaEndOverlap OnPenAreaEndOverlap;
	
	UFUNCTION(BlueprintPure)
	bool IsPlayerInsideArea();

protected:

	UPROPERTY()
	FPDWPenAreaBounds PenAreaBounds {};

	void BeginPlay() override;

};
