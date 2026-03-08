// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "PDWVehicleInteractionComponent.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWVehicleInteractionComponent : public UPDWInteractionComponent
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

	void OnCharacterPossessed() override;

public:
	
	void RemoveAllowedInteraction(const FGameplayTagContainer& inContainer) override;


	void SetAllowedInteraction(const FGameplayTagContainer& inContainer) override;

};
