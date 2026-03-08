// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "NebulaFlowSaveUserFSMState.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowSaveUserFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

protected:

	UFUNCTION()
	void SaveLoop();

	UFUNCTION()
	void OnSaveCompleted(bool bResult);

	FDelegateHandle SaveGameHandle;
};