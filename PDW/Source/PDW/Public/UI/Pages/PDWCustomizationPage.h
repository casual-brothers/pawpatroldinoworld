// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Pages/PDWUIBasePage.h"
#include "UI/Widgets/Customization/PDWCollectionWidget.h"
#include "Data/PDWPupConfigurationAsset.h"
#include "Managers/PDWCustomizationSubsystem.h"
#include "UI/Widgets/Customization/PDWCustomizationNavButton.h"
#include "PDWCustomizationPage.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWCustomizationPage : public UPDWUIBasePage
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCollectionWidget* CustomizationCollection;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCollectionWidget* PupCollection;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCollectionWidget* CustomizationCollectionP2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCollectionWidget* PupCollectionP2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCollectionWidget* DinoCollection;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCustomizationNavButton* CustomizationNavButtonP1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWCustomizationNavButton* CustomizationNavButtonP2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DinoName;
		
	UPROPERTY(EditAnywhere)
	FName SelectSoundPup = "HUD_ApplyCustomizationPup";
	UPROPERTY(EditAnywhere)
	FName SelectSoundVehicle = "HUD_ApplyCustomizationVehicle";
	UPROPERTY(EditAnywhere)
	FName SelectSoundDino = "HUD_ApplyCustomizationDino";

public:

	void InizializeDataPup(APDWPlayerController* PlayerController);

	void SwitchPupToVehicle(APDWPlayerController* PlayerController);

	void InizializeDataDino(FGameplayTag DinoTag);
};
