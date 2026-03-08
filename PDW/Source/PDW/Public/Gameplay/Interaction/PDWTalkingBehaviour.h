// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "PDWTalkingBehaviour.generated.h"

class UFlowComponent;

/**
 * 
 */
UCLASS()
class PDW_API UPDWTalkingBehaviour : public UPDWInteractionBehaviour
{
	GENERATED_BODY()
	
protected:
	void ExecuteBehaviour_Implementation() override;

	UFUNCTION()
	void OnStateTrigger();

	UPROPERTY()
	UFlowComponent* FlowComp = nullptr;

	void StopBehaviour_Implementation() override;

};
