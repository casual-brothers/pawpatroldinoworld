// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "GameplayTagContainer.h"
#include "PDWRyderMenuPage.generated.h"

class UHorizontalBox;
class UNebulaFlowBaseButton;
class UPDWMinigameListItem;
class USizeBox;
class URichTextBlock;
class UTextBlock;
class UVerticalBox;

UENUM(BlueprintType)
enum class EPDWRyderMenuState : uint8
{
	None,
	ModeSelection,
	Minigames
};

/**
 * Base class for the Ryder Menu Page, where player can choose to go into customization or replay minigames
 */
UCLASS(MinimalAPI, Abstract)
class UPDWRyderMenuPage : public UNebulaFlowBasePage
{
	GENERATED_BODY()
	
public:

	void ShowSelection();
	void ShowMinigames();

	EPDWRyderMenuState GetCurrentMenuState() { return CurrentMenuState; }
	FGameplayTagContainer GetMinigameIds() const { return CurrentMinigameIds; }

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void FillMinigamesList();
	void ClearMinigamesList();

	UFUNCTION()
	void OnMinigameSelected(UNebulaFlowBaseButton* SelectedButton);

	/** Setup Properties */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TMap<EPDWRyderMenuState, FText> DialogueTexts{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<UPDWMinigameListItem> MinigameListItemWidgetClass{ nullptr };


	/** Widget Bindings */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<URichTextBlock> LabelDialogue{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> PanelModes{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox> PanelMinigames{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> ListMinigames{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelNoMinigames{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UNebulaFlowBaseButton> BtnCustomization{ nullptr };

private:

	EPDWRyderMenuState CurrentMenuState{ EPDWRyderMenuState::None };

	FGameplayTagContainer CurrentMinigameIds{ FGameplayTagContainer::EmptyContainer };
};
