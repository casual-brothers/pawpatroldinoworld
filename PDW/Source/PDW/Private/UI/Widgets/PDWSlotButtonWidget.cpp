// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWSlotButtonWidget.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Data/PDWGameSettings.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPDWSlotButtonWidget::OnButtonFocused()
{
	Super::OnButtonFocused();
	if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
	{
		EventSubsystem->OnSlotOnFocus.Broadcast(SlotIndex);
	}
}

void UPDWSlotButtonWidget::SetupFromSlot(const FGameProgressionSlot& ProgressionSlot)
{
	bUsed = true;	
	OverlayNoSave->SetVisibility(ESlateVisibility::Collapsed);
	OverlaySaveSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	DateText->SetText(FText::AsDate(ProgressionSlot.SlotMetaData.LastSaved));
	MissionText->SetText(ProgressionSlot.PlayerData.CurrentQuestDescription);
	HourText->SetText(FText::AsTime(ProgressionSlot.SlotMetaData.LastSaved));
	FPDWAreaInfo AreaInfo = UPDWGameSettings::GetAreaInfo(ProgressionSlot.PlayerData.LastVisitedAreaId);
	AreaText->SetText(AreaInfo.AreaName);
	const FPupUIInfo& PupInfo = UPDWGameSettings::GetPupInfo(ProgressionSlot.PlayerData.SelectedPup);
	PupIcon->SetBrushFromSoftTexture(PupInfo.PupImage);
	OnSlotUsed(bUsed);
	SlotIndex = ProgressionSlot.SlotMetaData.SlotIndex;
}

void UPDWSlotButtonWidget::SetupEmpty()
{
	bUsed = false;
	OverlayNoSave->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	OverlaySaveSlot->SetVisibility(ESlateVisibility::Collapsed);
	Button->SetIsEnabled(false);
	OnSlotUsed(bUsed);
}

bool UPDWSlotButtonWidget::CheckEnable(bool bIsNewGame)
{
	Button->SetIsEnabled(bIsNewGame || bUsed);
	return bIsNewGame || bUsed;
}