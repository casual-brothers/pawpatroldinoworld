// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "GameOptionsState.generated.h"

class UGameOptionsPage;
class UNebulaFlowDialog;
class UPDWEventSubsytem;
class UPDWRemappingActionButton;

UCLASS(MinimalAPI, Abstract)
class UGameOptionsState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	virtual void OnFSMStateExit_Implementation() override;
	virtual	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)override;

	UPROPERTY(EditAnywhere)
	FName AskConfirmationDialogID = FName("AreYouSure");

protected:

	void OnResetSettingsDialog(FString InResponse);
	
	UFUNCTION()
	void OnOverlayClosed();

	UFUNCTION()
	void OnClosedSwitchRemap(bool bSingleJoyCon);

	void ResetSettings();

	UFUNCTION()
	void OnRemappingButtonClick(UPDWRemappingActionButton* RemappingButton);

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UGameOptionsPage> GameOptionsPage{ nullptr };

	//UPROPERTY(Transient, DuplicateTransient)
	//TObjectPtr<UPDWRemappingActionButton> ButtonWhoRequestKeyChange{ nullptr };

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowDialog> ResetSettingsDialog{ nullptr };

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UPDWEventSubsytem> EventSubsystemRef{ nullptr };

	bool bInsideCategory = false;

	bool bIsRemapping = false;
	

};
