// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "GameOptionsBaseSelectorWidget.h"
#include "GameOptionsSelectorWidget.generated.h"

struct FGameOptionsBaseData;
class UTextBlock;
class USlider;
class UImage;
class UProgressBar;


UCLASS(MinimalAPI, Abstract)
class UGameOptionsSelectorWidget : public UGameOptionsBaseSelectorWidget
{
	GENERATED_BODY()
	
#pragma region Setup

public:

	virtual void NativeConstruct() override;
	
	void Setup(FInstancedStruct& InSettingsData) override;	
	void SetupChangeGameOptionsOnNavigation() override;

#pragma endregion Setup

#pragma region UI and Animation

protected:

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	void SetColorsAndVisibility(bool bOnFocus);
	void UpdateArrowsVisibility(bool bOnFocus);
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

#pragma endregion UI and Animation

#pragma region Functionalities

public:

	void ResetValue() override;
		
	void ConfirmValue() override;

protected:

	UFUNCTION()
	UWidget* OnIncrease(EUINavigation NewNavigation);
	
	UFUNCTION()
	UWidget* OnDecrease(EUINavigation NewNavigation);

	UFUNCTION()
	void OnGameOptionsChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex);

	virtual void ChangeValue(bool bIncrease);
	void AfterValueChanged();

#pragma endregion Functionalities

#pragma region Colors

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor BackgroundNormalColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor BackgroundOnFocusColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FSlateColor GameOptionsNameTextNormalColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FSlateColor GameOptionsNameTextOnFocusColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FSlateColor GameOptionsValueTextNormalColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FSlateColor GameOptionsValueTextOnFocusColor;

#pragma endregion Colors

#pragma region BindedWidgets

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> GameOptionsNameText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> GameOptionsValueText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USlider> GameOptionsValueSlider;	
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> GameOptionsValueProgressBar;	
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> LeftArrow;	
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> RightArrow;	

#pragma endregion BindedWidgets

#pragma region Data

	FInstancedStruct GameOptionsData;

	FGameOptionsBaseData* BaseGameOptionsData;
		
#pragma endregion Data

#pragma region Blueprint events

protected:


	UFUNCTION(BlueprintImplementableEvent, meta = (Diplayname = "OnIncrease"))
	void BP_OnSetup(float PercentageValue);

	UFUNCTION(BlueprintImplementableEvent, meta = (Diplayname = "OnIncrease"))
	void BP_OnIncrease(float PercentageValue);

	UFUNCTION(BlueprintImplementableEvent, meta = (Diplayname = "OnDecrease"))
	void BP_OnDecrease(float PercentageValue);

	UFUNCTION(BlueprintImplementableEvent, meta = (Diplayname = "OnDecrease"))
	void BP_OnReset(float PercentageValue);

#pragma endregion Blueprint events

};

