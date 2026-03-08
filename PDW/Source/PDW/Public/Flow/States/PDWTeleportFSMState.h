// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "GameplayTagContainer.h"
#include "PDWTeleportFSMState.generated.h"

/**
 * Base class for the Teleport FMS State, where player can choose to quick travel to another area of the game
 */
UCLASS(MinimalAPI, Abstract)
class UPDWTeleportFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere)
	FName OnTeleportStartAudioID = "";

	virtual void OnFSMStateEnter_Implementation(const FString& InOption /* = FString("") */) override;
	virtual void OnFSMStateExit_Implementation();
	virtual void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;
	
	void OnPopupDismissed(const FString& InResponse);

	UFUNCTION()
	void OnTeleportComplete();

	UFUNCTION()
	void OnTransitionOutEnd();

private:

	UPROPERTY(Transient, DuplicateTransient)
	UNebulaFlowDialog* CurrentPopupDialog{ nullptr };

	FGameplayTag SelectedTeleportLocation{ FGameplayTag::EmptyTag };

	bool bIsTeleporting = false;
};