// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Pages/PDWUIBasePage.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "Delegates/Delegate.h"
#include "Managers/PDWHUDSubsystem.h"
#include "PDWBaseHUD.generated.h"

class UNebulaFlowNavbarButton;
class UPDWMinigameNavButton;
class UPDWDialogueBaseWidget;
class UPDWTutorialNavButton;

USTRUCT()
struct PDW_API FPDWButtonCallbacks
{
	GENERATED_USTRUCT_BODY()

public:

		FNavButtonTriggerCallback TriggerCallback = FNavButtonTriggerCallback();
		FNavButtonProgressCallback ProgressCallback = FNavButtonProgressCallback();
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWBaseHUD : public UPDWUIBasePage
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	virtual void InitHUD(const TArray<APlayerController*>& Players);
	
	UFUNCTION(BlueprintCallable)
	virtual void HideHUD();

	virtual void ManageInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack = FNavButtonTriggerCallback(), FNavButtonProgressCallback InProgressCallBack = FNavButtonProgressCallback());
	
	UFUNCTION(BlueprintCallable)
	virtual const EHUDType GetHUDType() const {return HUDType;};

	UFUNCTION(BlueprintCallable)
	virtual void ManageTriggerDialogue(const FConversation& Conversation);

	UFUNCTION(BlueprintCallable)
	virtual void ManageEndCurrentDialogue();

	UFUNCTION()
	virtual void ManageTutorialInputIcon(const FPDWTutorialButtonData& InputIconData);

	UFUNCTION()
	virtual void ManageEndTutorialInputIcon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_CheckHUDConfiguration();

protected:

	void NativeDestruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPanelWidget* NavButtonActionContainer;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWDialogueBaseWidget* HUDComment;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPanelWidget* TutorialNavButtonContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPDWTutorialNavButton> TutorialNavButtonClass;

	UPROPERTY()
	UPDWTutorialNavButton* TutorialNavButtonIcon = nullptr;

	UPROPERTY()
	bool bHUDCommentIsPlaying = false;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitHUD(const TArray<APlayerController*>& Players);

	UFUNCTION()
	void OnNavIconTrigger(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance);

	UFUNCTION()
	void OnNavIconProgress(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance, float PreviousTime, float CurrentTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	EHUDType HUDType;

	TMap<UPDWMinigameNavButton*, FPDWButtonCallbacks> CallbackToCallByButton;

	void OnMultiplayerStateChanged() override;

	UFUNCTION()
	virtual void OnHUDCommentEnd();

};
