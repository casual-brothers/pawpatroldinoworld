// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "PDWPupSwapBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWPupSwapBehaviour : public UPDWInteractionBehaviour
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FGameplayTag PupTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bTurnIntoVehicle = false;

	void ExecuteBehaviour_Implementation() override;
};
