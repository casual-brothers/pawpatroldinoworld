// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Pages/PDWRyderMenuPage.h"
#include "Components/HorizontalBox.h"
#include "Components/RichTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/Widgets/PDWMinigameListItem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"


void UPDWRyderMenuPage::ShowSelection()
{
	if (CurrentMenuState == EPDWRyderMenuState::ModeSelection)
	{
		return;
	}

	CurrentMenuState = EPDWRyderMenuState::ModeSelection;

	LabelDialogue->SetText(DialogueTexts.FindRef(CurrentMenuState));

	ClearMinigamesList();

	PanelMinigames->SetVisibility(ESlateVisibility::Collapsed);
	PanelModes->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	UNebulaFlowUIFunctionLibrary::SetUserFocus(this, BtnCustomization);
}

void UPDWRyderMenuPage::ShowMinigames()
{
	if (CurrentMenuState == EPDWRyderMenuState::Minigames)
	{
		return;
	}

	CurrentMenuState = EPDWRyderMenuState::Minigames;

	LabelDialogue->SetText(DialogueTexts.FindRef(CurrentMenuState));

	PanelModes->SetVisibility(ESlateVisibility::Collapsed);
	PanelMinigames->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	FillMinigamesList();
}

void UPDWRyderMenuPage::NativeConstruct()
{
	Super::NativeConstruct();

	ShowSelection();
}

void UPDWRyderMenuPage::NativeDestruct()
{
	ClearMinigamesList();

	Super::NativeDestruct();
}

void UPDWRyderMenuPage::FillMinigamesList()
{
	ClearMinigamesList();

	if (!ensureMsgf(MinigameListItemWidgetClass, TEXT("MinigameListItemClass not set!")))
	{
		return;
	}

	TArray<FPDWMinigameInfo> AvailableMinigames = UPDWGameplayFunctionLibrary::GetPlayedMinigames(this);

	for (const FPDWMinigameInfo& MinigameInfo : AvailableMinigames)
	{
		UPDWMinigameListItem* MinigameListItem = CreateWidget<UPDWMinigameListItem>(this, MinigameListItemWidgetClass);
		if (!ensureMsgf(MinigameListItem, TEXT("Failed to create a UPDWMinigameListItem instance!")))
		{
			continue;
		}
		
		MinigameListItem->InitMinigameData(MinigameInfo);
		MinigameListItem->ButtonFocused.AddUniqueDynamic(this, &UPDWRyderMenuPage::OnMinigameSelected);

		ListMinigames->AddChildToVerticalBox(MinigameListItem);
	}

	if (ListMinigames->GetChildrenCount() > 0)
	{
		LabelNoMinigames->SetVisibility(ESlateVisibility::Collapsed);
		UNebulaFlowUIFunctionLibrary::SetUserFocus(this, ListMinigames->GetChildAt(0));
	}
	else
	{
		LabelNoMinigames->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPDWRyderMenuPage::ClearMinigamesList()
{
	for (UWidget* Widget : ListMinigames->GetAllChildren())
	{
		if (UPDWMinigameListItem* ListItem = Cast<UPDWMinigameListItem>(Widget))
		{
			ListItem->ButtonFocused.RemoveAll(this);
		}
	}

	ListMinigames->ClearChildren();
}

void UPDWRyderMenuPage::OnMinigameSelected(UNebulaFlowBaseButton* SelectedButton)
{
	CurrentMinigameIds = Cast<UPDWMinigameListItem>(SelectedButton)->GetIdentityTags();
}
