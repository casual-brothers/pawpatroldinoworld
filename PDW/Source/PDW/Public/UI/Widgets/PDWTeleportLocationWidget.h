// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowBaseButton.h"
#include "GameplayTagContainer.h"
#include "PDWTeleportLocationWidget.generated.h"

class UImage;
class UOverlay;
class UTextBlock;

/**
 * Base class for a Travel Location Widget, used in the Teleport Page
 */
UCLASS(MinimalAPI, Abstract)
class UPDWTeleportLocationWidget : public UNebulaFlowBaseButton
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SelectArea(const bool bIsSelected);

	FORCEINLINE FGameplayTag GetAreaId() const { return TeleportAreaId; }

protected:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AnimPlayerIcon();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AnimQuestIcon();

	/** Setup Properties */

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Setup")
	FGameplayTag TeleportAreaId{ FGameplayTag::EmptyTag };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Setup")
	TSoftObjectPtr<UTexture2D> UnselectedAreaImage{ nullptr };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Setup")
	TSoftObjectPtr<UTexture2D> SelectedAreaImage{ nullptr };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Setup")
	FText LockedAreaName;


	/** Widget Bindings */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UOverlay> PanelNewArea{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelAreaName{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> AreaImage{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ImageLocked{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ImageSelection{ nullptr };
};
