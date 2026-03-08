// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "FSM/NebulaFlowBaseFSMState.h"

#include "NebulaFlowBaseStateInitCondition.generated.h"



UCLASS(BlueprintType,Blueprintable)
class NEBULAFLOW_API UNebulaFlowBaseStateInitCondition : public UObject
{
	GENERATED_UCLASS_BODY()

	friend class UNebulaFlowBaseFSM;
	friend class UNebulaFlowBaseFSMState;


protected:	

	virtual void Initialize(const UNebulaFlowBaseFSMState* Owner);

	virtual bool CheckResolvedCondition(float deltaTime);

	bool IsConditionInitialized(){return bIsInitialized;}

private:

	bool bIsInitialized = false;

};