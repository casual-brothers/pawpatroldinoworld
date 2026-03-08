// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "NebulaFlowStartFSMState.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowStartFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	virtual void OnFSMStateExit_Implementation() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup - Switch")
	bool bEnableSingleJoycon = false;

private:

};