// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Actors/PDWCollectibleBase.h"
#include "PDWPupTreat.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API APDWPupTreat : public APDWCollectibleBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DESIGN")
	int32 PointsGained = 1;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "DESIGN")
	UMaterialInterface* PickedUpMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	int32 CollectibleIndex = -1;

	void PostEditImport() override;

protected:
	void OnPickUpStart(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController) override;


	void OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController) override;


	void BeginPlay() override;

};
