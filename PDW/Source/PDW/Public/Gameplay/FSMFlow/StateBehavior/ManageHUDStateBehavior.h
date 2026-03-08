// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/StateBehaviors/NebulaFlowBaseStateBehavior.h"
#include "Managers/PDWHUDSubsystem.h"
#include "ManageHUDStateBehavior.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class PDW_API UManageHUDStateBehavior : public UNebulaFlowBaseStateBehavior
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	EHUDControls HUDControl = EHUDControls::None;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="HUDControl == EHUDControls::Show || HUDControl == EHUDControls::Create", EditConditionHides))
	EHUDType HUDType = EHUDType::Gameplay;

	virtual void OnEnter_Implementation();
};