// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/GameOptionsSelectorWidget.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Image.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "Data/GameOptionsData.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Components/ProgressBar.h"


#pragma region Setup

void UGameOptionsSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnGameOptionsChanged.AddUniqueDynamic(this, &UGameOptionsSelectorWidget::OnGameOptionsChanged);
	}
	// Important: set this widget focusable
	//bIsFocusable = true;
}

void UGameOptionsSelectorWidget::Setup(FInstancedStruct& InSettings)
{
	GameOptionsData = InSettings;
	BaseGameOptionsData = GameOptionsData.GetMutablePtr<FGameOptionsBaseData>();
	GameOptionsNameText->SetText(BaseGameOptionsData->GetDisplayName());
	if (GameOptionsValueText)
	{
		GameOptionsValueText->SetText(BaseGameOptionsData->GetCurrentValueText());
	}
	if (GameOptionsValueSlider)
	{
		GameOptionsValueSlider->SetValue(BaseGameOptionsData->GetCurrentPercent());
	}
	if (GameOptionsValueProgressBar)
	{
		GameOptionsValueProgressBar->SetPercent(BaseGameOptionsData->GetCurrentPercent());
	}
	SetColorsAndVisibility(false);
	BP_OnSetup(BaseGameOptionsData->GetCurrentPercent());
}

void UGameOptionsSelectorWidget::SetupChangeGameOptionsOnNavigation()
{
	FCustomWidgetNavigationDelegate NavDel = FCustomWidgetNavigationDelegate();
	NavDel.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UGameOptionsSelectorWidget, OnIncrease));
	SetNavigationRuleCustom(EUINavigation::Right, NavDel);

	FCustomWidgetNavigationDelegate NavDel2 = FCustomWidgetNavigationDelegate();
	NavDel2.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UGameOptionsSelectorWidget, OnDecrease));
	SetNavigationRuleCustom(EUINavigation::Left, NavDel2);
}

#pragma endregion Setup

#pragma region UI and Animation

FReply UGameOptionsSelectorWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	SetColorsAndVisibility(true);
	//OnFocusedGameOptions.Broadcast(this);
	return FReply::Unhandled();
}

void UGameOptionsSelectorWidget::SetColorsAndVisibility(bool bOnFocus)
{
	GameOptionsNameText->SetColorAndOpacity(bOnFocus ? GameOptionsNameTextOnFocusColor : GameOptionsNameTextNormalColor);
	Background->SetColorAndOpacity(bOnFocus ? BackgroundOnFocusColor : BackgroundNormalColor);
	if (GameOptionsValueText)
	{
		GameOptionsValueText->SetColorAndOpacity(bOnFocus ? GameOptionsValueTextOnFocusColor : GameOptionsValueTextNormalColor);
	}
	UpdateArrowsVisibility(bOnFocus);
}

void UGameOptionsSelectorWidget::UpdateArrowsVisibility(bool bOnFocus)
{
	if (LeftArrow)
	{
		bool bIsVisible = bOnFocus && (BaseGameOptionsData->GetCycle() || BaseGameOptionsData->GetCurrentValueIndex() != 0);
		LeftArrow->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (RightArrow)
	{
		bool bIsVisible = bOnFocus && (BaseGameOptionsData->GetCycle() || BaseGameOptionsData->GetCurrentValueIndex() < BaseGameOptionsData->GetNumberOfValues() - 1);
		RightArrow->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UGameOptionsSelectorWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	SetColorsAndVisibility(false);
}

#pragma endregion UI and Animation

#pragma region Functionalities

void UGameOptionsSelectorWidget::ResetValue()
{
	BaseGameOptionsData->SetToDefault();
	AfterValueChanged();
	BP_OnReset(BaseGameOptionsData->GetCurrentPercent());
}

void UGameOptionsSelectorWidget::ConfirmValue()
{
	if (FBoolGameOptionsData* BoolGameOptionsData = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>())
	{
		if (BaseGameOptionsData->GetCurrentValueIndex() > 0)
		{
			OnDecrease(EUINavigation::Left);
		}
		else
		{
			OnIncrease(EUINavigation::Right);
		}
	}
}

UWidget* UGameOptionsSelectorWidget::OnIncrease(EUINavigation NewNavigation)
{
	if (BaseGameOptionsData->GetNumberOfValues() <= 1)	return this;
	// change option ++
	ChangeValue(true);
	BP_OnIncrease(BaseGameOptionsData->GetCurrentPercent());
	return this;
}

UWidget* UGameOptionsSelectorWidget::OnDecrease(EUINavigation NewNavigation)
{
	if (BaseGameOptionsData->GetNumberOfValues() <= 1)	return this;
	// change option --
	ChangeValue(false);
	BP_OnDecrease(BaseGameOptionsData->GetCurrentPercent());
	return this;
}

void UGameOptionsSelectorWidget::OnGameOptionsChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex)
{
	if (SettingsId == BaseGameOptionsData->GetId())
	{
		BaseGameOptionsData->SetCurrentValueIndex(CurrentValueIndex);
		if (GameOptionsValueText)
		{
			GameOptionsValueText->SetText(BaseGameOptionsData->GetCurrentValueText());
		}
		if (GameOptionsValueSlider)
		{
			GameOptionsValueSlider->SetValue(BaseGameOptionsData->GetCurrentPercent());
		}		
		if (GameOptionsValueProgressBar)
		{
			GameOptionsValueProgressBar->SetPercent(BaseGameOptionsData->GetCurrentPercent());
		}
	}
}

void UGameOptionsSelectorWidget::ChangeValue(bool bIncrease)
{
	if (BaseGameOptionsData->GetCycle())
	{
		BaseGameOptionsData->SetCurrentValueIndex(BaseGameOptionsData->GetCurrentValueIndex() + (bIncrease ? 1 : -1));
	}
	else
	{
		BaseGameOptionsData->SetCurrentValueIndex(FMath::Clamp(BaseGameOptionsData->GetCurrentValueIndex() + (bIncrease ? 1 : -1), 0, BaseGameOptionsData->GetNumberOfValues() - 1));
	}
	AfterValueChanged();
}

void UGameOptionsSelectorWidget::AfterValueChanged()
{
	if (GameOptionsValueText)
	{
		GameOptionsValueText->SetText(BaseGameOptionsData->GetCurrentValueText());
	}
	if (GameOptionsValueSlider)
	{
		GameOptionsValueSlider->SetValue(BaseGameOptionsData->GetCurrentPercent());
	}
	if (GameOptionsValueProgressBar)
	{
		GameOptionsValueProgressBar->SetPercent(BaseGameOptionsData->GetCurrentPercent());
	}
	UpdateArrowsVisibility(true);
	
	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnPressed);
	UGameOptionsFunctionLibrary::ChangeGameOptions(GetWorld(), GameOptionsData);
}

#pragma endregion Functionalities