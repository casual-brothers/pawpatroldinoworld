// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/PDWTeleportLocationWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/QuestSubsystem.h"



void UPDWTeleportLocationWidget::SelectArea(const bool bIsSelected)
{
	if (UPDWDataFunctionLibrary::IsAreaVisited(this, TeleportAreaId))
	{
		AreaImage->SetBrushFromSoftTexture(bIsSelected ? SelectedAreaImage : UnselectedAreaImage);
	}
	
	ImageSelection->SetVisibility(bIsSelected ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UPDWTeleportLocationWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	AreaImage->SetBrushFromSoftTexture(UnselectedAreaImage);
}

void UPDWTeleportLocationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UPDWDataFunctionLibrary::IsAreaVisited(this, TeleportAreaId))
	{
		AreaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ImageLocked->SetVisibility(ESlateVisibility::Collapsed);

		const FPDWAreaInfo AreaInfo = UPDWGameSettings::GetAreaInfo(TeleportAreaId);

		LabelAreaName->SetText(AreaInfo.AreaName);
		PanelNewArea->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		AreaImage->SetVisibility(ESlateVisibility::Hidden);
		ImageLocked->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PanelNewArea->SetVisibility(ESlateVisibility::Hidden);
	}

	// check if player is here
	if (UPDWDataFunctionLibrary::GetPlayerAreaId(this) == TeleportAreaId)
	{
		BP_AnimPlayerIcon();
	}

	// check if active quest is here
	UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	if (!ensureMsgf(QuestSubsystem, TEXT("Couldn't retrieve QuestSubsystem while opening the travel menu!")))
	{
		return;
	}

	const UBaseFlowQuest* CurrentQuest = QuestSubsystem->GetCurrentQuest();
	if (CurrentQuest != nullptr && CurrentQuest->GetQuestArea() == TeleportAreaId)
	{
		BP_AnimQuestIcon();
	}
}