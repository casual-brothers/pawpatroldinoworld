// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateBehaviors/NebulaFlowBaseStateBehavior.h"
#include "GameplayTagContainer.h"
#include "ManageInputStateBehavior.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UManageInputStateBehavior : public UNebulaFlowBaseStateBehavior
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bRemoveInput = false;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "bRemoveInput == false", Categories = "Input.MappingContext"))
	FGameplayTag MapTagToAdd;

	void OnEnter_Implementation() override;
};
