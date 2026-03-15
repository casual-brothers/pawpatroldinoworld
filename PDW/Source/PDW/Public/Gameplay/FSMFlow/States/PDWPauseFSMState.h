// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "Containers/Ticker.h"
#include "PDWPauseFSMState.generated.h"

/**
 * Base class for the Pause Menu FSM State
 */
UCLASS(MinimalAPI, Abstract)
class UPDWPauseFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
public:

	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateExit_Implementation() override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

protected:
	UFUNCTION()
	void ToggleJoinMultiplayer();

	UFUNCTION()
	void ToggleGuestStreamPlay();

	UFUNCTION()
	void OnGuestWaitingDialogResponse(FString Response);

	void OnGuestSessionChanged(bool bIsSessionActive);
	void OnGuestWaitingTimedOut();
	void RefreshGuestNavbarButton();

	UFUNCTION()
	void OnModalResponse(FString Response);
	
	UFUNCTION()
	void OnGoToMainMenuDialogResponse(FString InResponse);

	UFUNCTION()
	void BackToGameplay();

	void RegisterGuestStreamPlayNavbarButton();

	void DefineStateOwner() override;
	
	/** Setup Properties */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	FName DefaultJoinPlayerModalId{ NAME_None };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	FName SwitchJoinPlayerModalId{ NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	FGameplayTag TutorialFactTag{};

private:

	UPROPERTY(Transient)
	UNebulaFlowDialog* DialogToHide{ nullptr };

	UPROPERTY(Transient)
	UNebulaFlowDialog* GuestWaitingDialog{ nullptr };
		
	UPROPERTY(EditAnywhere)
	FName AskConfirmationDialogID = FName("BackToMainMenu");

	UPROPERTY(EditAnywhere)
	FName GuestWaitingDialogID = FName("GuestStreamPlayWaiting");

	UPROPERTY(EditAnywhere, Category = "Setup")
	float GuestWaitingTimeout = 30.0f;

	UPROPERTY(EditAnywhere)
	FGameplayTag QuestTutorialTag = {};

	UPROPERTY()
	bool bIsTutorialActive = false;

	FTimerHandle GuestWaitingTimeoutHandle;
};
