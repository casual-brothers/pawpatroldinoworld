// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "Managers/PDWUIManager.h"
#include "PDWUIBasePage.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class PDW_API UPDWUIBasePage : public UNebulaFlowBasePage
{
	GENERATED_BODY()
	
protected:
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* DebugCurrentVisualizationText;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageConfiguration")
	EVisualizationType CurrentVisualization = EVisualizationType::Singleplayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageConfiguration")
	TObjectPtr<UPDWUIPageElementsData> PageElementsDataAsset;

	void NativePreConstruct() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
#if WITH_EDITORONLY_DATA
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	virtual void ChangePageVisualization(EVisualizationType NewVisualization);
	
	UFUNCTION()
	virtual void OnElementDataChange(FName WidgetName, FUIElementData& NewData);

	UFUNCTION()
	virtual void OnMultiplayerStateChanged();

	UFUNCTION()
	virtual void OnGameOptionChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex);
};
