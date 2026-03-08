// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/NebulaFlowNavbarDataStructures.h"
#include "NebulaFlowNavButtonBase.generated.h"

class UNebulaFlowNavigationIcon;
class UTextBlock;
class UButton;
class UPanelWidget;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class NEBULAFLOW_API UNebulaFlowNavButtonBase : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	UFUNCTION(BlueprintCallable, Category = "FLOW|UI")
	virtual void InitializeButton(FNavbarButtonData NewButtonData);
	
	UFUNCTION(BlueprintCallable, Category = "FLOW|UI")
	virtual void UninitializeButton();
	
	UFUNCTION(BlueprintCallable)
	virtual void ChangeButtonEnabled(const bool bActivate);

protected:

#pragma region WidgetBinding

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UNebulaFlowNavigationIcon> NavigationIcon {};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ButtonText {};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClickableButton {};
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ProgressBarPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> CircularProgress;
#pragma endregion

#pragma region Configuration

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	FNavbarButtonData NavbarButtonData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	FName ValueParameterName = "Percentage";

	#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Editor Only")
	bool bShowProgressBarPanel = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Editor Only")
	FText ButtonDisplayText {};
	#endif

#pragma endregion

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> OnStartedAction {};

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> OnCompleteAction{};
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsTimeTriggerEvent = false;

	UPROPERTY()
	bool bIsOneShot = false;

	UPROPERTY()
	TObjectPtr<ANebulaFlowPlayerController> InputTriggerSender {};

	UPROPERTY()
	float CurrentTime = 0.f;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	virtual void OnClick();

	UFUNCTION()
	virtual void OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC);

	UFUNCTION()
	virtual void OnInputStarted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC);

	UFUNCTION()
	virtual void OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC);

	UFUNCTION()
	virtual void OnInputCompleted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC);

	UFUNCTION()
	virtual void OnInputCancelled(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC);

	virtual void OnPlayerLogin(AGameModeBase* InGameMode, APlayerController* InPlayerController);
	virtual void OnPlayerLogout(AGameModeBase* InGameMode, AController* InController);

	UFUNCTION()
	virtual void OnButtonTriggered();

	UFUNCTION(BlueprintImplementableEvent, meta = (Diplayname = "OnButtonTriggered"))
	void BP_OnButtonTrigger();

	UFUNCTION(BlueprintImplementableEvent, meta = (Diplayname = "OnButtonEnabledChanged"))
	void BP_OnButtonEnabledChanged(const bool bIsButtonEnabled);

#pragma region Deprecated
	UFUNCTION(meta=(DeprecatedFunction))
	virtual void OnAnyKeyPressed(FKey Key, bool bIsGamepad, APlayerController* Sender);
	
	UFUNCTION(meta=(DeprecatedFunction))
	virtual void OnAnyKeyReleased(FKey Key, bool bIsGamepad, APlayerController* Sender);

	UFUNCTION(meta=(DeprecatedFunction))
	void OnInputPressReceived();

	UFUNCTION(meta=(DeprecatedFunction))
	void OnInputReleaseReceived();

	bool InputPressed = false;

	UPROPERTY()
	TMap<APlayerController*, bool> PressedMap{};

	FDelegateHandle InputPressActionHandler;

	FDelegateHandle InputReleasedActionHandler;
	
	bool bIsNavbarButtonEnabled = true;

	int32 PressHandle;

	int32 ReleaseHandle;
#pragma endregion
};
