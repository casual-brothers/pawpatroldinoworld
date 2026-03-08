// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "PDWMainMenuState.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWMainMenuState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString(""));

	virtual	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;


protected:

	void Continue();

	void DeleteGame();


	bool LoadFromSlotID(int32 slotID);

	UFUNCTION()
	void OnSlotFocused(int32 slotID);
	UFUNCTION()
	void OnPageReady();
	void StartNewGame(int32 slotID);

	void QuitGame();
	void LoadGame();
	void Proceed();
	UPROPERTY(EditAnywhere, Category = "Setup")
	FText ContinueLabel{};

	UPROPERTY(EditAnywhere, Category = "Setup")
	FName DeleteSlotDialogID = FName("AreYouSure");

	UPROPERTY(EditAnywhere, Category = "Setup")
	FName OverwriteSlotDialogueID = FName("AreYouSure");

	UPROPERTY(EditAnywhere, Category = "Setup")
	FName ExitGameDialogID = FName("ExitGame");
	
	UPROPERTY(EditAnywhere, Category = "Setup")
	FName LoadGameDialogID = FName("LoadGame");

	bool bIsNewGame{ false };
	bool bIsSelectingSlots{ false };
	bool bPageReady{ false };

private:
		
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowDialog> DeleteSlotDialog{ nullptr };

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowDialog> OverwriteSlotDialog{ nullptr };

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowDialog> QuitGameDialog{ nullptr };

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowDialog> LoadGameDialog{ nullptr };

	int32 LastSelectedSlotID{ -1 };
	
	int32 LastFocusedSlotID{ -1 };
};
