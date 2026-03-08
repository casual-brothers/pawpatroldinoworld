// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "PDWCreditsFSMState.generated.h"

/**
 * Base class for the Credits FSM State
 */
UCLASS(MinimalAPI, Abstract)
class UPDWCreditsFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption /* = FString("") */) override;
	virtual void OnFSMStateExit_Implementation() override;
	virtual void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

protected:

	UFUNCTION()
	void OnCreditsEndReached();	
};
