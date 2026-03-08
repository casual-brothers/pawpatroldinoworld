// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "UI/Widgets/NebulaFlowBaseWidget.h"
#include "Animation/UMGSequencePlayer.h"
#include "PDWRemappingWidgetOverlay.generated.h"

class UPDWRemappingActionButton;
class UNebulaFlowNavigationIcon;
class UCanvasPanel;
class UTextBlock;

UCLASS(meta=(BlueprintSpawnableComponent))
class PDW_API UPDWRemappingWidgetOverlay : public UNebulaFlowBaseWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	
	UFUNCTION()
	void OnClosedSwitchRemap(bool bSingleJoyCon);

	UFUNCTION()
	void OnRemappingButtonClick(UPDWRemappingActionButton* RemappingButton);
	
	void OnOpenAnimationFinished(FWidgetAnimationState& AnimationState);

	UFUNCTION()
	void OnRemappingButtonNewKeyChosen(UPDWRemappingActionButton* RemappingButton, FKey InputKey);
	
	void OnCloseAnimationFinished(FWidgetAnimationState& AnimationState);

	UFUNCTION()
	void OnRemappingButtonInvalidNewKeyChosen(UPDWRemappingActionButton* RemappingButton);

	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RemapCanvas{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UNebulaFlowNavigationIcon> RemapIcon{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> InvalidKeyText{ nullptr };
		
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextRemapActionName{ nullptr };
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> OpenAnimation;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> CloseAnimation;
};

