// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Pages/PDWUIBasePage.h"
#include "Components/TextBlock.h"
#include "Managers/PDWEventSubsytem.h"
#include "Blueprint/WidgetTree.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/HUD/PDWUIBaseElement.h"
#include "Managers/PDWUIManager.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"

void UPDWUIBasePage::NativePreConstruct()
{
	Super::NativePreConstruct();
#if WITH_EDITOR
	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);
	for (UWidget* Widget : AllWidgets)
	{
		if (UPDWUIBaseElement* BaseElement = Cast<UPDWUIBaseElement>(Widget))
		{
			BaseElement->OnCurrentElementDataChange.AddUniqueDynamic(this, &ThisClass::OnElementDataChange);
		}
	}

	ChangePageVisualization(CurrentVisualization);
#endif
}

void UPDWUIBasePage::NativeConstruct()
{
	Super::NativeConstruct();
#if WITH_EDITOR
	if (DebugCurrentVisualizationText)
	{
		DebugCurrentVisualizationText->SetVisibility(ESlateVisibility::Collapsed);
	}
#endif
	UPDWEventSubsytem::Get(this)->OnGameOptionsChanged.AddUniqueDynamic(this, &ThisClass::OnGameOptionChanged);
	UPDWEventSubsytem::Get(GetWorld())->OnMultiplayerStateChange.AddUniqueDynamic(this, &ThisClass::OnMultiplayerStateChanged);
	OnMultiplayerStateChanged();
}

void UPDWUIBasePage::NativeDestruct()
{
	UPDWEventSubsytem::Get(this)->OnGameOptionsChanged.RemoveDynamic(this, &ThisClass::OnGameOptionChanged);
	UPDWEventSubsytem::Get(GetWorld())->OnMultiplayerStateChange.RemoveDynamic(this, &ThisClass::OnMultiplayerStateChanged);
	Super::NativeDestruct();
}

#if WITH_EDITOR
void UPDWUIBasePage::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == "CurrentVisualization")
	{
		ChangePageVisualization(CurrentVisualization);
	}
}
#endif

void UPDWUIBasePage::ChangePageVisualization(EVisualizationType NewVisualization)
{
	if (!PageElementsDataAsset)
	{
		return;
	}
	
	bool bIsLargeHUDActive = UGameOptionsFunctionLibrary::GetEnhancedHUDEnabled();
#if WITH_EDITOR
	if (IsDesignTime())
	{
		bIsLargeHUDActive = false;
	}
#endif
	if (bIsLargeHUDActive)
	{
		if(NewVisualization == EVisualizationType::Singleplayer)
		NewVisualization = EVisualizationType::SingleplayerLarge;

		if(NewVisualization == EVisualizationType::Multiplayer)
		NewVisualization = EVisualizationType::MultiplayerLarge;
	}
	CurrentVisualization = NewVisualization;
	

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);
	for (UWidget* Widget : AllWidgets)
	{
		if (UPDWUIBaseElement* BaseElement = Cast<UPDWUIBaseElement>(Widget))
		{
			if (PageElementsDataAsset->ContainsWidgetName(BaseElement->GetElementID()))
			{
				BaseElement->ChangeCurrentConfiguration(CurrentVisualization, PageElementsDataAsset->GetElementDataByName(BaseElement->GetElementID(), CurrentVisualization));
			}
		}
	}

#if WITH_EDITOR
	if (DebugCurrentVisualizationText)
	{
		UEnum* EnumPtr = StaticEnum<EVisualizationType>();
		FString Message = EnumPtr->GetNameStringByValue((int64)NewVisualization);
		DebugCurrentVisualizationText->SetText(FText::AsCultureInvariant(UPDWUIFunctionLibrary::InsertSpacesBeforeUppercase(Message)));
	}
#endif
}

void UPDWUIBasePage::OnElementDataChange(FName WidgetName, FUIElementData& NewData)
{
#if WITH_EDITOR
	if (!PageElementsDataAsset)
	{
		return;
	}

	if (PageElementsDataAsset->ContainsWidgetName(WidgetName))
	{
		PageElementsDataAsset->SetElementDataByName(WidgetName, CurrentVisualization, NewData);
	}
	else
	{
		FWidgetVisualizationSet NewSet = FWidgetVisualizationSet();
		NewSet.WidgetName = WidgetName;

		FWidgetVisualizationData SingleplayerVisualization;
		SingleplayerVisualization.VisualizationType = EVisualizationType::Singleplayer;
		SingleplayerVisualization.UIElementData = NewData;
		NewSet.Visualizations.Add(SingleplayerVisualization);

		FWidgetVisualizationData MultiplayerVisualization;
		SingleplayerVisualization.VisualizationType = EVisualizationType::Multiplayer;
		SingleplayerVisualization.UIElementData = NewData;
		NewSet.Visualizations.Add(SingleplayerVisualization);
		
		PageElementsDataAsset->WidgetVisualizations.Add(NewSet);
	}

	PageElementsDataAsset->Modify();
	PageElementsDataAsset->MarkPackageDirty();
#endif
}

void UPDWUIBasePage::OnMultiplayerStateChanged()
{
	ChangePageVisualization(UPDWGameplayFunctionLibrary::IsMultiplayerOn(this) ? EVisualizationType::Multiplayer : EVisualizationType::Singleplayer);
}

void UPDWUIBasePage::OnGameOptionChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex)
{
	OnMultiplayerStateChanged();
}