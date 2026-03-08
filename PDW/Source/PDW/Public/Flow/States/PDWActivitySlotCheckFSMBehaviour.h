// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateBehaviors/NebulaFlowBaseStateBehavior.h"
#include "PDWActivitySlotCheckFSMBehaviour.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBehaviorTrigger : uint8
{
	OnEnter = 0,
	OnExit = 1,
	OnBeforeEnter = 2,
};


UCLASS()
class PDW_API UPDWActivitySlotCheckFSMBehaviour : public UNebulaFlowBaseStateBehavior
{
	GENERATED_BODY()
	
	protected:

	UPROPERTY(EditAnywhere)
	EBehaviorTrigger TriggeringStatus = EBehaviorTrigger::OnEnter;

	void OnEnter_Implementation() override;


	void OnExit_Implementation() override;
};
