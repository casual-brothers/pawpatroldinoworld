// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "NebulaFlowLoadUserFSMState.generated.h"


UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowLoadUserFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

	virtual void OnFSMStateExit_Implementation() override;

protected:

	UFUNCTION()
	virtual void OnUserLoaded(int ControllerId);


};