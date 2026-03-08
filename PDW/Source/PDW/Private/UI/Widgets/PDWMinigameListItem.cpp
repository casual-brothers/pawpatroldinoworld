// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Widgets/PDWMinigameListItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/PDWGameSettings.h"

void UPDWMinigameListItem::InitMinigameData(const FPDWMinigameInfo& MinigameInfo)
{
	const FPDWAreaInfo AreaInfo = UPDWGameSettings::GetAreaInfo(MinigameInfo.Area);
	LabelAreaName->SetText(AreaInfo.AreaName);
	
	LabelMinigameName->SetText(MinigameInfo.Name);
	
	const FEntity PupConfig = UPDWGameSettings::GetEntityConfigForTag(MinigameInfo.AllowedPup);
	ImagePup->SetBrushFromSoftTexture(PupConfig.PupUIInfo.PupImage);

	// store mini game Ids for invoking replay
	IdentityTags = MinigameInfo.IdentityTags;
}
