// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Core/NebulaFlowEnhancedPlayerInput.h"
#include "PDWRemappingWidget.generated.h"

class UButton;
class UScrollBox;
class UEnhancedInputLocalPlayerSubsystem;
class UEnhancedInputUserSettings;
class UPDWRemappingActionButton;
class UPDWInputsData;
class UPDWRemappingCategoryTitle;

UCLASS(meta=(BlueprintSpawnableComponent))
class PDW_API UPDWRemappingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetFocusOnRemapButtons();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bindwidget))
	TObjectPtr<UButton> ListenKeyButton{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bindwidget))
	TObjectPtr<UScrollBox> ControllerMapping{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bindwidget))
	TObjectPtr<UScrollBox> KeyboardMapping{ nullptr };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Setup")
	TSubclassOf<UPDWRemappingActionButton> RemapWidgetClass{};
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Setup")
	TSubclassOf<UPDWRemappingCategoryTitle> RemapWidgetCategoryTitleClass{};
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Setup")
	float SpaceBetweenCategories{ 50.0f };

	UPROPERTY(EditAnywhere)
	FName NotRemappableKeyID = FName("NotRemappableKey");

	void Setup();
	void Empty();

	void ResetInputs();

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void OnRemappingButtonNewKeyChosen(UPDWRemappingActionButton* RemappingButton, FInputChord InputChord);

	UFUNCTION()
	void OnRemappingButtonClick(UPDWRemappingActionButton* RemappingButton);

	UFUNCTION()
	void GenerateRemapWidgets(bool bIsKeyboard);

	void SetupFocusOnInputTypeChanged(bool bIsGamepad);

	void OnAnyKeyPressed(FKey Key, bool bIsGamepad, APlayerController* Sender);

	void ChangeBinding(FKey InKey);

	UFUNCTION()
	void OnOverlayClosed();
	UFUNCTION()
	void OnOverlayOpened();

	void MapKeys(TArray<FName>& Actions, FKey NewKey, EPlayerMappableKeySlot KeySlot, UEnhancedInputUserSettings* EIUS);

	void FilterOutActionsNotInPairedIMCs(TArray<FName>& Actions, UInputMappingContext* IMC);
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> EISubsystem{};

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UEnhancedInputUserSettings> UserSettings{};

	UPROPERTY(Transient, DuplicateTransient)
	TArray<TObjectPtr<UEnhancedInputUserSettings>> AllActiveUserSettings{};

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UPDWInputsData> InputsData{};

	UPROPERTY(Transient, DuplicateTransient)
	TArray<FName> ActionsAlreadyMappedWithNewKey;
	UPROPERTY(Transient, DuplicateTransient)
	TArray<FName> ActionsAlreadyMappedWithOldKey;

	UPROPERTY(Transient, DuplicateTransient)
	FName ActionMappingName{};

	bool bIsUsingGamePad{ false };
	bool bCanSelectKey{ false };

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowEnhancedPlayerInput> EnhancedInput{};
		
	UPROPERTY(EditAnywhere)
	FMargin GameOptionsPadding;

private:

	void GenerateRemapWidgets();

	FString FindActionDisplayName(const FName& ActionName);

	UFUNCTION()
	bool OnKeySelected(FInputChord InputChord);

	UPROPERTY()
	UPDWRemappingActionButton* RemappingActionButton{};

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<ULocalPlayer> LocalPlayer{ nullptr };

	UPROPERTY()
	TArray<TObjectPtr<UPDWRemappingActionButton>> ButtonListKeyboard{};

	UPROPERTY()
	TArray<TObjectPtr<UPDWRemappingActionButton>> ButtonListGamepad{};

	FKey ContendedKey{}; // in case we need to swap mapping , we hold the key here

	EPlayerMappableKeySlot OldKeySlot{};

	FDelegateHandle InputHandler;


};

