// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "PDWMinigameCompletedFSMState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigameCompletedFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_BODY()

public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

};
