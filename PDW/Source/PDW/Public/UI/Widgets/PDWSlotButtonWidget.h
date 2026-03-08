// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowBaseButton.h"
#include "Data/PDWSaveStructures.h"
//#include "UI/Core/PDWButton.h"

#include "PDWSlotButtonWidget.generated.h"

class UImage;
class UOverlay;
class UTextBlock;
class UButton;

/**
 * Base class for the SaveSlot Button used to pick a saved slot
 */
UCLASS(MinimalAPI, Abstract)
class UPDWSlotButtonWidget : public UNebulaFlowBaseButton
{
	GENERATED_BODY()

public:

	void SetupFromSlot(const FGameProgressionSlot& ProgressionSlot);
	void SetupEmpty();

	bool CheckEnable(bool bIsNewGame);
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnSlotUsed(const bool bIsUsed);

	virtual void OnButtonFocused() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UOverlay> OverlayNoSave{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UOverlay> OverlaySaveSlot{ nullptr };
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AreaText{ nullptr };
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MissionText{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DateText{ nullptr };
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HourText{ nullptr };
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> PupIcon{ nullptr };
		
private:

	bool bUsed = false;

	int32 SlotIndex;
};


