// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "FSM/States/NebulaFlowStartFSMState.h"
#include "PDWStartFSMState.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent))
class PDW_API UPDWStartFSMState : public UNebulaFlowStartFSMState
{
	GENERATED_BODY()
	
public:
	
	UPDWStartFSMState(){}

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup - Switch")
	int32 MaxSwitchPlayers = 2;

};

