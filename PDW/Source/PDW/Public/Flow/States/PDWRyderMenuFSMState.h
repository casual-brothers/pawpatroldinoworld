// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "PDWRyderMenuFSMState.generated.h"

/**
 * Base class for the Ryder Menu FSM State - Allows player to choose between customization or replaying a minigame
 */

 class UNebulaFlowDialog;

UCLASS(MinimalAPI, Abstract)
class UPDWRyderMenuFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
protected:

	virtual void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

	UFUNCTION()
	void OnRequestToJoinAnswered(FString inAnswer, APDWPlayerController* inController);

	UFUNCTION()
	void ReplayMinigame();

	UPROPERTY()
	UNebulaFlowDialog* DialogInstance = nullptr;
};
