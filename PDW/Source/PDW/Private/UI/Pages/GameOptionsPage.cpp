// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Pages/GameOptionsPage.h"
#include "UI/Widgets/GameOptionsCategoryWidget.h"
#include "UI/Widgets/GameOptionsSelectorWidget.h"
#include "Components/PanelWidget.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Components/ScrollBox.h"
#include "Data/GameOptionsDeveloperSettings.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "Components/Image.h"
#include "FunctionLibraries/ConsoleVarLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "UI/Widgets/PDWRemappingWidget.h"
#include "PDWGameInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "BinkMediaTexture.h"
#include "BinkMediaPlayer.h"
#include "MediaPlayer.h"
#include "FileMediaSource.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"


void UGameOptionsPage::EnterCurrentCategory()
{
	if (GetOwningPlayer())
	{
		if (CurrentCategory == EGameOptionsCategory::Controls)
		{
			RemappingWidget->SetFocusOnRemapButtons();

			GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionConfirm());			
			GetPageNavbar()->AddNavbarButton(UPDWGameSettings::GetUIActionRemap(), ENavElementPosition::RIGHT);
		}
		else
		{
			if (CurrentCategory != EGameOptionsCategory::Eula)
			{
				GetPageNavbar()->AddNavbarButton(UPDWGameSettings::GetUIActionReset(), ENavElementPosition::RIGHT);
			}
			else
			{				
				GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionConfirm());
			}
			UNebulaFlowUIFunctionLibrary::SetUserFocus(this, GameOptionsContainer->GetChildAt(0), GetOwningPlayer(), false);
		}
		GEngine->ForceGarbageCollection();
	}
}

void UGameOptionsPage::ExitCategory()
{
	if (!GetOwningPlayer())
	{
		return;
	}
	GetPageNavbar()->AddNavbarButton(UPDWGameSettings::GetUIActionConfirm(), ENavElementPosition::RIGHT);
	GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionRemap());			
	GetPageNavbar()->RemoveNavbarButton(UPDWGameSettings::GetUIActionReset());

	GEngine->ForceGarbageCollection();

	for (TSoftObjectPtr<UGameOptionsCategoryWidget> CategoryWidget : CategoryWidgets)
	{
		if (CategoryWidget.Get() && CategoryWidget.Get()->Category == CurrentCategory)
		{
			UNebulaFlowUIFunctionLibrary::SetUserFocus(this, CategoryWidget.Get(),  GetOwningPlayer(), false);
			return;
		}
	}
}

void UGameOptionsPage::ResetCurrentGameOptions()
{
	if (CurrentGameOptionsWidget)
	{
		CurrentGameOptionsWidget->ResetValue();
	}
	else if(CurrentCategory == EGameOptionsCategory::Controls)
	{
		RemappingWidget->ResetInputs();
	}
}

void UGameOptionsPage::ConfirmCurrentGameOptions()
{
	if (CurrentGameOptionsWidget)
	{
		CurrentGameOptionsWidget->ConfirmValue();
	}
}

void UGameOptionsPage::NativePreConstruct()
{
	Super::NativePreConstruct();

	InitCategories();
	InitCurrentCategory();

	if (bBackgroundImageVisible)
	{
		UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
		if (PersistentUser == nullptr)
			return;

		int currentSlotID = PersistentUser->GetCurrentUseSlotIndex();

		bool bSuccess = false;
		FGameplayTag LastAreaVisited = FGameplayTag::RequestGameplayTag("Map.DinoPlains");
		FGameProgressionSlot& SaveSlot = PersistentUser->GetCurrentUsedProgressionSlot(bSuccess);
		if (bSuccess)
		{
			LastAreaVisited = SaveSlot.PlayerData.LastVisitedAreaId;
		}


#if PLATFORM_SWITCH
		UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(SwitchMaterial.LoadSynchronous(), this);
		BackgroundImage->SetBrushFromMaterial(DynMat);
#else
		UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(DefaultMaterial.LoadSynchronous(), this);
		BackgroundImage->SetBrushFromMaterial(DynMat);
#endif
		float Gamma = UConsoleVarLibrary::GetConsoleFloat(FName("r.TonemapperGamma"));
		DynMat->SetScalarParameterValue(FName("Gamma"), Gamma);

		if (BackgroundVideosMaterial.Contains(LastAreaVisited))
		{
#if PLATFORM_SWITCH
			auto SwitchVideo = BackgroundVideosMaterial[LastAreaVisited].SwitchVideoToPlay.LoadSynchronous();
			if (SwitchVideo)
			{
				MediaPlayer->OpenSource(SwitchVideo);
			}

#else
			UBinkMediaPlayer* Video = BackgroundVideosMaterial[LastAreaVisited].BinkVideo.LoadSynchronous();
			if (Video)
			{
				BinkMediaTexture->SetMediaPlayer(Video);
				Video->InitializePlayer();
				Video->Play();
			}
#endif
		}
	}
	else
	{
		BackgroundImage->SetVisibility(ESlateVisibility::Hidden);
	}
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnGameOptionsChanged.AddUniqueDynamic(this, &UGameOptionsPage::OnGameOptionsChanged);
	}
#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.AddUniqueDynamic(this, &UGameOptionsPage::OnClosedSwitchRemap);
#endif
}

void UGameOptionsPage::NativeDestruct()
{
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	EventSubsystem->OnGameOptionsChanged.RemoveAll(this);
#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.RemoveAll(this);
#endif

}

void UGameOptionsPage::OnGameOptionsChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex)
{
	if (SettingsId == EGameOptionsId::Gamma)
	{
		if (BackgroundImage)
		{
			if (auto* DynMat = BackgroundImage->GetDynamicMaterial())
			{
				float Gamma = UConsoleVarLibrary::GetConsoleFloat(FName("r.TonemapperGamma"));
				DynMat->SetScalarParameterValue(FName("Gamma"), Gamma);
			}
		}
	}
}

void UGameOptionsPage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bNeedUpdateScrollBar)
	{
		UpdateScrollbarVisibility();
	}
}

void UGameOptionsPage::InitCategories()
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (!ensureMsgf(GameOptionsDevSet->CategoryWidget.Get(), TEXT("add Category Widget in Game Options Data in config")))
	{
		return;
	}
	for (int32 i = CategoryContainer->GetChildrenCount() - 1; i >= 0; i--)
	{
		CategoryContainer->RemoveChildAt(i);

	}
	CategoryWidgets.Empty();
	for (EGameOptionsCategory Category : Categories)
	{
		if (!UGameOptionsFunctionLibrary::HasCategoryAnyGameOptions(Category) && Category != EGameOptionsCategory::Controls)
		{
			continue;
		}
#if PLATFORM_SWITCH
		if(Category == EGameOptionsCategory::Controls && UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0))
		{
			continue;
		}
#endif

		UGameOptionsCategoryWidget* CategoryWidget = CreateWidget<UGameOptionsCategoryWidget>(GetWorld(), GameOptionsDevSet->CategoryWidget.Get());
		if (!CategoryWidget) return;

		CategoryWidget->Setup(Category);
		CategoryContainer->AddChild(CategoryWidget);
		CategoryWidget->OnFocusedCategory.AddUniqueDynamic(this, &UGameOptionsPage::ChangeCategory);


		CategoryWidget->SetPadding(CategoryPadding);
		if (Category == Categories[0])
		{
			if (GetWorld()->GetFirstPlayerController())
			{
				UNebulaFlowUIFunctionLibrary::SetUserFocus(this, CategoryWidget,  GetOwningPlayer(), false);
			}
		}
		CategoryWidgets.Add(CategoryWidget);
	}
	for (int32 i = 0; i < CategoryContainer->GetChildrenCount(); i++)
	{
		UGameOptionsCategoryWidget* Child = Cast<UGameOptionsCategoryWidget>(CategoryContainer->GetChildAt(i));

		Child->SetAllNavigationRules(EUINavigationRule::Stop, "");
		// Link to siblings
		if (i - 1 >= 0)
		{
			Child->SetNavigationRuleExplicit(EUINavigation::Up, CategoryContainer->GetChildAt(i - 1));
		}
		if (i + 1 <= CategoryContainer->GetChildrenCount() - 1)
		{
			Child->SetNavigationRuleExplicit(EUINavigation::Down, CategoryContainer->GetChildAt(i + 1));
		}
	}
}

void UGameOptionsPage::InitCurrentCategory()
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	for (int32 i = GameOptionsContainer->GetChildrenCount() - 1; i >= 0; i--)
	{
		GameOptionsContainer->RemoveChildAt(i);
	}

	if(CurrentCategory == EGameOptionsCategory::Controls)
	{
		if (RemappingWidget)
		{
			GameOptionsContainer->SetVisibility(ESlateVisibility::Hidden);
			RemappingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			return;
		}
	}
	else
	{
		if (RemappingWidget)
		{
			GameOptionsContainer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			RemappingWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	TArray<FInstancedStruct> GameOptionsByCategory;
#if WITH_EDITOR
	// to preview in game options page while working
	if (!GWorld->IsGameWorld())
	{
		UGameOptionsFunctionLibrary::GetGameOptionsByCategory(nullptr, CurrentCategory, GameOptionsByCategory);
	}
	else
#endif //WITH_EDITOR
		UGameOptionsFunctionLibrary::GetGameOptionsByCategoryWithSaves(GetWorld(), CurrentCategory, GameOptionsByCategory);

	for (FInstancedStruct& GameOptions : GameOptionsByCategory)
	{
		if (!GameOptions.IsValid() || !GameOptions.GetScriptStruct()->IsChildOf(FGameOptionsBaseData::StaticStruct()))
		{
			continue;
		}
		FGameOptionsBaseData* GameOptionsData = GameOptions.GetMutablePtr<FGameOptionsBaseData>();
		
		if(HiddenGameOptions.Contains(GameOptionsData->GetId()))
		{
			continue;
		}
		TSubclassOf<UGameOptionsSelectorWidget> WidgetClass = GameOptionsData->GetOverrideSelector();
		if (!WidgetClass)
		{
			FName ClassName = GameOptions.GetScriptStruct()->GetFName();
			if (!ensureMsgf(GameOptionsDevSet->DefaultSelectorWidgets.Contains(ClassName), TEXT("add defaulty selector widgets to game options settings for %s"), *ClassName.ToString()))
			{
				continue;
			}
			WidgetClass = GameOptionsDevSet->DefaultSelectorWidgets[ClassName];
		}

		UGameOptionsSelectorWidget* SelectorWidget = CreateWidget<UGameOptionsSelectorWidget>(GetWorld(), WidgetClass.Get());
		if (!SelectorWidget) return;

		SelectorWidget->Setup(GameOptions);
		SelectorWidget->OnFocusedGameOptions.AddUniqueDynamic(this, &UGameOptionsPage::OnFocusGameOptionsWidget);

		GameOptionsContainer->AddChild(SelectorWidget);

		SelectorWidget->SetPadding(GameOptionsPadding);
	}

	GameOptionsContainer->SynchronizeProperties();

	for (int32 i = 0; i < GameOptionsContainer->GetChildrenCount(); i++)
	{
		UGameOptionsSelectorWidget* Child = Cast<UGameOptionsSelectorWidget>(GameOptionsContainer->GetChildAt(i));

		Child->SetAllNavigationRules(EUINavigationRule::Stop, "");
		// Link to siblings
		if (i - 1 >= 0)
		{
			Child->SetNavigationRuleExplicit(EUINavigation::Up, GameOptionsContainer->GetChildAt(i - 1));
		}
		if (i + 1 <= GameOptionsContainer->GetChildrenCount() - 1)
		{
			Child->SetNavigationRuleExplicit(EUINavigation::Down, GameOptionsContainer->GetChildAt(i + 1));
		}
		Child->SetupChangeGameOptionsOnNavigation();

	}

	bNeedUpdateScrollBar = true;
}


void UGameOptionsPage::UpdateScrollbarVisibility()
{
	UScrollBox* ScrollBox = Cast<UScrollBox>(GameOptionsContainer);
	if (!ScrollBox) return;

	const FVector2D ViewportSize = ScrollBox->GetCachedGeometry().GetLocalSize();

	// if its not loaded yet
	if (ViewportSize.Y <= 0.f)
	{
		return;
	}
	bNeedUpdateScrollBar = false;
	const FVector2D ContentSize = ScrollBox->GetDesiredSize();

	bool bNeedsScroll = ContentSize.Y > ViewportSize.Y;

	auto Style = ScrollBox->GetWidgetBarStyle();
	FLinearColor ThumbColor = Style.NormalThumbImage.TintColor.GetSpecifiedColor();
	ThumbColor.A = bNeedsScroll ? 1.f : 0.f;
	Style.NormalThumbImage.TintColor = ThumbColor;
	ScrollBox->SetWidgetBarStyle(Style);
}



void UGameOptionsPage::ChangeCategory(EGameOptionsCategory Category)
{
	CurrentCategory = Category;
	InitCurrentCategory();
	CurrentGameOptionsWidget = nullptr;
}

void UGameOptionsPage::OnFocusGameOptionsWidget(UGameOptionsBaseSelectorWidget* GameOptionsWidget)
{
	CurrentGameOptionsWidget = GameOptionsWidget;
}

void UGameOptionsPage::OnClosedSwitchRemap(bool bSingleJoyCon)
{
#if PLATFORM_SWITCH
	InitCategories();
	InitCurrentCategory();
#endif
}

void UGameOptionsPage::InitControlsCategory()
{
	GameOptionsContainer->SetVisibility(ESlateVisibility::Hidden);
	RemappingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}