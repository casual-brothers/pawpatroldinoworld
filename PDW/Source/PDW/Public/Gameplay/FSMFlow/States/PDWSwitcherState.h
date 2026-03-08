// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWSwitcherState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWSwitcherState : public UNebulaFlowBaseFSMState
{

	GENERATED_BODY()
	
public:

	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

	void OnFSMStateExit_Implementation() override;

protected:

	UPROPERTY()
	bool DoingTransition = false;

	UPROPERTY()
	FPDWTriggerActionData PendingAction;

};
