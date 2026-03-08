// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "PDWMinigameFSMState.generated.h"

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigameFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_BODY()

public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;
};
