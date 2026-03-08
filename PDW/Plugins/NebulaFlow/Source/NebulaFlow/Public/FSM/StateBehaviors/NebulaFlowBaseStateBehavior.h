// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "NebulaFlowBaseStateBehavior.generated.h"

class UNebulaFlowBaseFSMState;

UCLASS(Abstract,Blueprintable, EditInlineNew, DefaultToInstanced)
class NEBULAFLOW_API UNebulaFlowBaseStateBehavior : public UObject
{
	GENERATED_UCLASS_BODY()

	friend class UNebulaFlowBaseFSMState;
	friend class UNebulaFlowBaseFSM;


protected:

	void InitStateBehavior (UNebulaFlowBaseFSMState* inStateOwner){ StateOwner = inStateOwner; }

	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
		void BeforeOnEnter();

	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
		void OnEnter();

	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
		void OnExit();

	UPROPERTY()
	UNebulaFlowBaseFSMState* StateOwner  = nullptr;


};