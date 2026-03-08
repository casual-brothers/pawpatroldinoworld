// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "UI/Widgets/NebulaFlowBaseButton.h"
#include "Data/PDWGameplayStructures.h"
#include "EnhancedActionKeyMapping.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "PDWRemappingActionButton.generated.h"

class UPDWRemappingActionButton;
class UNebulaFlowNavigationIcon;
class UTextBlock;

UCLASS(meta = (BlueprintSpawnableComponent))
class PDW_API UPDWRemappingActionButton : public UNebulaFlowBaseButton
{
	GENERATED_BODY()

public:

	void InitializeWidget(const TSet<FPlayerKeyMapping>& KeyMapping, bool IsKeyboardButton, EPlayerMappableKeySlot In_Slot, UInputMappingContext* InIMC);

	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	TObjectPtr<UTextBlock> InputName{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	TObjectPtr<UNebulaFlowNavigationIcon> NavIcon{ nullptr };
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UInputMappingContext> IMC;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsKeyboardButton = false;

	const TSet<FPlayerKeyMapping>* StoredKeyMapping{};

	EPlayerMappableKeySlot Slot;

	UPROPERTY(Transient, DuplicateTransient)
	const UInputAction* ActionName { };

	FName OverriddenActionName{ NAME_None };

	bool bIsRemapping;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputAction> ConfirmInputAction;

	UPROPERTY(BlueprintReadOnly)
	FPDWRemappingAction InputAction;

	UPROPERTY(BlueprintReadWrite)
	FEnhancedActionKeyMapping StoredKey;

	virtual void NativeDestruct()override;

	virtual void OnButtonClicked() override;
	virtual void OnButtonFocused() override;


	UFUNCTION()
	void CheckIfUnfocus(UPDWRemappingActionButton* ButtonCurrentlyInFocus);
	
	UFUNCTION()
	void OnRemappingButtonNewKeyChosen(UPDWRemappingActionButton* RemappingButton, FKey InputKey);
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnfocused();

};

