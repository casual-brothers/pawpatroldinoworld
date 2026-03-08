// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "GameplayTagContainer.h"
#include "PDWPauseMenuPage.generated.h"

class UImage;
class UTextBlock;
class UPDWHUDQuestLog;
class UPDWDinoPenCollection;
class UPDWUnlockableBox;

/**
 * Base class for the Pause Menu Page
 */
UCLASS(MinimalAPI, Abstract)
class UPDWPauseMenuPage : public UNebulaFlowBasePage
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void PrevTab();

	UFUNCTION(BlueprintCallable)
	void NextTab();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_StartTutorial();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SkipTutorialDialogue();

protected:

	virtual void NativeConstruct() override;

	void FillAreaInfo();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTabChanged(int32 SelectedTabIndex);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateCustomizationsCount(int32 CustomizationsCount);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateVehiclesCount(int32 VehiclesCount);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetCompletedQuestAt(const int32 QuestIndex, const bool bIsCompleted);

	/** Setup Properties */

	UPROPERTY(EditAnywhere, Category = "Setup")
	FText MissinDinoName{};

	UPROPERTY(EditAnywhere, Category = "Setup")
	TSoftObjectPtr<UTexture2D> MissingDinoIcon{ nullptr };

	
	/** Widget Bindings */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUserWidget> BtnPrevTab{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUserWidget> BtnNextTab{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelPupTreats{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelAreaName{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelCompletedQuests{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ImageAreaPreview{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelDinoName{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWUnlockableBox> UnlockableBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWHUDQuestLog> PanelQuestLog{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWDinoPenCollection> PanelEggs{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWDinoPenCollection> PanelCustomizations{ nullptr };

private:

	TArray<FGameplayTag> AvailableAreas{};

	int32 CurrentTabIndex{ 0 };
};
