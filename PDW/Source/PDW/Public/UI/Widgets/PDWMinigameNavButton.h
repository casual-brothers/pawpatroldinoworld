// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowNavButtonBase.h"
#include "FMODBlueprintStatics.h"
#include "PDWMinigameNavButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerComplete, UPDWMinigameNavButton*, Button, const FInputActionInstance&, Instance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnProgressUpdate, UPDWMinigameNavButton*, Button, const FInputActionInstance&, Instance, float, PreviousTime, float, CurrentTime);

/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigameNavButton : public UNebulaFlowNavButtonBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	FOnTriggerComplete OnTriggerComplete;

	UPROPERTY()
	FOnProgressUpdate OnProgressUpdate;

	UFUNCTION(BlueprintImplementableEvent)
	void ActiveHint();

	void InitializeButton(FNavbarButtonData NewButtonData) override;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerIndicator(bool bIsPlayerOne) {BP_ShowPlayerIndicator(bIsPlayerOne);};

protected:
	
	UPROPERTY()
	FInputActionInstance TriggerActionInstance;

	UPROPERTY(EditAnywhere)
	bool bSelfRemove = true;

	UPROPERTY()
	bool bAlreadyTriggered = false;

	UPROPERTY(EditAnywhere, Category = "Audio")
	FName ProgressSoundName = "MinigameRepeatingButton";

	UPROPERTY(EditAnywhere, Category = "Audio")
	FName ProgressSoundParameterName = "reapiting_button";

	UPROPERTY()
	FFMODEventInstance AudioInstance;

	void OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	
	UFUNCTION(BlueprintCallable)
	void CompleteMinigameButton();

	void OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitButton();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowPlayerIndicator"))
	void BP_ShowPlayerIndicator(bool bIsPlayerOne);
};
