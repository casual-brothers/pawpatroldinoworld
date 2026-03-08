// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/HUD/PDWMinigameHUD.h"
#include "PDWMinigamePlayersNavButton.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigamePlayersNavButton : public UUserWidget
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	UNebulaFlowNavbarButton* NavButton;

	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	UImage* PlayerImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Player1Texture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Player2Texture;

public:
	UFUNCTION(BlueprintCallable)
	void InitMinigamePlayersButton(const FPDWMinigameInputData& InputData);
};
