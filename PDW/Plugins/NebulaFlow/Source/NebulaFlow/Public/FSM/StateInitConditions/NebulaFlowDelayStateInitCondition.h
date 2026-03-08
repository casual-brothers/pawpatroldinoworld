// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "FSM/StateInitConditions/NebulaFlowBaseStateInitCondition.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "NebulaFlowDelayStateInitCondition.generated.h"



UCLASS(BlueprintType,Blueprintable)
class NEBULAFLOW_API UNebulaFlowDelayStateInitCondition : public UNebulaFlowBaseStateInitCondition
{
	GENERATED_UCLASS_BODY()

protected:	

	virtual void Initialize(const UNebulaFlowBaseFSMState* Owner) override;

	virtual bool CheckResolvedCondition(float deltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float DelayTime=10.f;

private:

	float Counter = 0.f;

};