// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Actors/PDWCollectibleBase.h"
#include "PDWInventoryCollectible.generated.h"

class UFlowComponent;

UCLASS()
class PDW_API APDWInventoryCollectible : public APDWCollectibleBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	UFlowComponent* FlowComp;

	UPROPERTY(EditAnywhere)
	bool DestroyIfAlreadyTaken = true;

protected:

	void OnPickUpStart(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController) override;


	void OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController) override;


	void BeginPlay() override;
	
};
