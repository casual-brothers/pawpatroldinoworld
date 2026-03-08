// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "UI/Pages/PDWUIBasePage.h"
#include "Data/GameOptionsData.h"
#include "Data/PDWGameplayStructures.h"
#include "GameOptionsPage.generated.h"

class UScrollBox;
class UGameOptionsCategoryWidget;
class UGameOptionsBaseSelectorWidget;
class UPDWRemappingWidget;
class UMediaPlayer;
class UBinkMediaTexture;

UCLASS(meta = (BlueprintSpawnableComponent))
class PDW_API UGameOptionsPage : public UPDWUIBasePage
{
	GENERATED_BODY()

public:

	UGameOptionsPage() {}

protected:

	void NativePreConstruct() override;
	void NativeDestruct() override;

	UFUNCTION()
	void OnGameOptionsChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex);
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void EnterCurrentCategory();
	void ExitCategory();
	void ResetCurrentGameOptions();
	void ConfirmCurrentGameOptions();

	EGameOptionsCategory GetCurrentCategory() { return CurrentCategory; }

protected:

	void InitCategories();
	void InitCurrentCategory();	

	bool bNeedUpdateScrollBar = false;
	void UpdateScrollbarVisibility();
	
	UFUNCTION()
	void ChangeCategory(EGameOptionsCategory Category);
	
	UFUNCTION()
	void OnFocusGameOptionsWidget(UGameOptionsBaseSelectorWidget* GameOptionsWidget);
	
	UFUNCTION()
	void OnClosedSwitchRemap(bool bSingleJoyCon);

	void InitControlsCategory();

	UPROPERTY(EditAnywhere)
	EGameOptionsCategory CurrentCategory;

	UPROPERTY(EditAnywhere)
	TArray<EGameOptionsId> HiddenGameOptions;

	UPROPERTY(EditDefaultsOnly)
	TArray<EGameOptionsCategory> Categories;
	
	UPROPERTY(EditAnywhere)
	bool bBackgroundImageVisible = true;
		
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FPDWMenuVideo> BackgroundVideosMaterial;
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterial> DefaultMaterial{ nullptr };

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterial> SwitchMaterial{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Video")
	UMediaPlayer* MediaPlayer{};

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UBinkMediaTexture> BinkMediaTexture{ nullptr };

	UPROPERTY(EditDefaultsOnly)
	FMargin CategoryPadding;

	UPROPERTY(EditDefaultsOnly)
	FMargin GameOptionsPadding;
		
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> BackgroundImage{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> CategoryContainer{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> GameOptionsContainer{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWRemappingWidget> RemappingWidget{ nullptr };
			
	UPROPERTY(Transient, meta = (BindWidget))
	TArray<TSoftObjectPtr<UGameOptionsCategoryWidget>> CategoryWidgets {};

	UPROPERTY(Transient)
	TObjectPtr<UGameOptionsBaseSelectorWidget> CurrentGameOptionsWidget;

};

