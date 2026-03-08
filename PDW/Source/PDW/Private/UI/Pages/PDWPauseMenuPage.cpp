// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Pages/PDWPauseMenuPage.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/HUD/PDWHUDQuestLog.h"
#include "UI/Widgets/DinoPen/PDWDinoPenCollection.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "UI/Widgets/PDWUnlockableBox.h"

void UPDWPauseMenuPage::NativeConstruct()
{
	Super::NativeConstruct();

	// fill stats
	const int32 CollectedPupTreats = UPDWDataFunctionLibrary::GetExpPoints(this);
	LabelPupTreats->SetText(FText::FromString(FString::FromInt(CollectedPupTreats)));

	const FCustomizationData CustomizationData = UPDWDataFunctionLibrary::GetCustomizationData(this);
	BP_UpdateCustomizationsCount(CustomizationData.PupCustomizations.Num());
	BP_UpdateVehiclesCount(CustomizationData.VehicleCustomizations.Num());

	// fill area info
	AvailableAreas = UPDWDataFunctionLibrary::GetVisitedAreaIds(this);

	CurrentTabIndex = AvailableAreas.Find(UPDWDataFunctionLibrary::GetPlayerAreaId(this));

	BtnPrevTab->SetVisibility(AvailableAreas.Num() > 1 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	BtnNextTab->SetVisibility(AvailableAreas.Num() > 1 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);

	BP_OnTabChanged(CurrentTabIndex);
	FillAreaInfo();
}

void UPDWPauseMenuPage::PrevTab()
{
	CurrentTabIndex = FMath::Clamp(CurrentTabIndex - 1, 0, AvailableAreas.Num() - 1);

	BP_OnTabChanged(CurrentTabIndex);
	FillAreaInfo();
}

void UPDWPauseMenuPage::NextTab()
{
	CurrentTabIndex = FMath::Clamp(CurrentTabIndex + 1, 0, AvailableAreas.Num()-1);

	BP_OnTabChanged(CurrentTabIndex);
	FillAreaInfo();
}

void UPDWPauseMenuPage::FillAreaInfo()
{
	if (AvailableAreas.Num() == 0)
	{
		return;
	}

	// show area name and image
	const FPDWAreaInfo AreaInfo = UPDWGameSettings::GetAreaInfo(AvailableAreas[CurrentTabIndex]);

	LabelAreaName->SetText(AreaInfo.AreaName);
	ImageAreaPreview->SetBrushFromSoftTexture(AreaInfo.PreviewImage);

	// show completed quests
	TArray<FQuestData> QuestData = UPDWGameplayFunctionLibrary::GetAreaQuests(this, AvailableAreas[CurrentTabIndex]);

	uint8 CompletedQuestsNum = 0;

	for (int32 i = 0; i < QuestData.Num(); ++i)
	{
		BP_SetCompletedQuestAt(i, QuestData[i].IsCompleted);

		if (QuestData[i].IsCompleted)
		{
			CompletedQuestsNum++;
		}
	}

	LabelCompletedQuests->SetText(FText::FromString(FString::FromInt(CompletedQuestsNum)));

	// show current quest objectives
	PanelQuestLog->InitQuestLog();
	
	// show current dinosaur and eggs
	UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>();
	if (!ensureMsgf(PaleoCenterSubsystem, TEXT("Couldn't retrieve PaleoCenterSubsystem!")))
	{
		return;
	}

	FPDWDinoPenInfo DinoPenInfo = PaleoCenterSubsystem->GetDinoPenInfoAndActivity(AreaInfo.DinoPenId);

	LabelDinoName->SetText(DinoPenInfo.IsPenUnlocked ? DinoPenInfo.DinoDisplayName : MissinDinoName);
	UnlockableBox->SetUnlockableIcon(DinoPenInfo.IsPenUnlocked ? DinoPenInfo.DinoDisplayIcon : MissingDinoIcon);
	UnlockableBox->SetUnlockableFound(DinoPenInfo.IsPenUnlocked);

	PanelEggs->InitDinoPenByEggs(DinoPenInfo.DinoPenEggs);
	PanelCustomizations->InitDinoPenByDecorations(DinoPenInfo.DinoPenDecorations);
}
