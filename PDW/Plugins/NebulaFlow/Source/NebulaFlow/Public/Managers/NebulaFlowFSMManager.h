// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "NebulaFlowManagerBase.h"
#include "NebulaFlowFSMManager.generated.h"


class UNebulaFlowBaseFSM;
class UNebulaFlowGameInstance;

UCLASS(BlueprintType,Blueprintable)
class NEBULAFLOW_API UNebulaFlowFSMManager :  public UNebulaFlowManagerBase
{
	
	GENERATED_UCLASS_BODY()

public:
	
	void InitManager(UNebulaFlowGameInstance* InstanceOwner);

	bool GetCurrentFSMLabel(FName& OutLabel, bool CleanAfterRead = true);

	void SetCurrentFSMLabel(FName InLabel)
	{
		CurrentFSMLabel.Reset();
		CurrentFSMLabel = InLabel;
	}

	void RegisterFSM(UNebulaFlowBaseFSM* FSM) {
		CurrentRunningFSMs.Add(FSM);
	}

	void UnregisterFSM(UNebulaFlowBaseFSM* FSM)
	{
		CurrentRunningFSMs.Remove(FSM);
	}

	FName GetCurrentFsmStateId();

	// #TODO_NebulaFlow remove unused array and store a single pointer
	UNebulaFlowBaseFSM* GetCurrentFSM() { return CurrentRunningFSMs.Num() > 0 ? CurrentRunningFSMs[0] : nullptr; }

private:

	TOptional<FName> CurrentFSMLabel;

	// #TODO_NebulaFlow remove unused array and store a single pointer
	UPROPERTY()
	TArray<UNebulaFlowBaseFSM*> CurrentRunningFSMs;

};