// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "PDWMinigameSetupFSMState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigameSetupFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_BODY()
	
public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;
};
