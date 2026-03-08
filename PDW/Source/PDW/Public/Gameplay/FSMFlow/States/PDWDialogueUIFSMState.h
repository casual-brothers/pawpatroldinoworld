// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "PDWDialogueUIFSMState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWDialogueUIFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
public:

	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

	void OnFSMStateExit_Implementation() override;

};
