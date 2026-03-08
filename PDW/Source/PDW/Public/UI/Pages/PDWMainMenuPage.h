// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Pages/PDWUIBasePage.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWMainMenuPage.generated.h"

class UNebulaFlowBaseButton;
class UPDWSlotButtonWidget;
class UBinkMediaPlayer;
class UBinkMediaTexture;
class UFileMediaSource;
class UMediaPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchToMainMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchToSlots);

/**
 * Base Class for the Slot Selection Page (Before Main Menu)
 */
UCLASS(MinimalAPI, Abstract)
class UPDWMainMenuPage : public UPDWUIBasePage
{
	GENERATED_BODY()

public:

	void Init(int ButtonSelectedSlot);
	void SwitchToSlotButtons(bool IsNewGame = false);
	void SwitchToMenuButtons(bool bWasLoadGame);


	UPROPERTY(BlueprintAssignable)
	FOnSwitchToMainMenu OnSwitchToMainMenu;

	UPROPERTY(BlueprintAssignable)
	FOnSwitchToSlots OnSwitchToSlots;

protected:

	virtual void NativeConstruct()override;
			
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FPDWMenuVideo> BackgroundVideosMaterial;
		
	UPROPERTY(EditAnywhere, Category = "Video")
	UMediaPlayer* MediaPlayer{};

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UBinkMediaTexture> BinkMediaTexture{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWSlotButtonWidget> BtnSlot0{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWSlotButtonWidget> BtnSlot1{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWSlotButtonWidget> BtnSlot2{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UNebulaFlowBaseButton> BtnContinue{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UNebulaFlowBaseButton> BtnNewGame{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UNebulaFlowBaseButton> BtnLoadGame{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UNebulaFlowBaseButton> BtnQuit{ nullptr };
	
	bool bCanContinue;
		
	UPROPERTY(EditAnywhere)
	UMaterial* SwitchMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial = nullptr;
};
