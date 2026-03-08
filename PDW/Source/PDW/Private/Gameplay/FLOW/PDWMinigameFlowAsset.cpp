// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWMinigameFlowAsset.h"

void UPDWMinigameFlowAsset::SetMinigameTag(const FGameplayTag& inMinigameTag)
{
	Configuration.MiniGameTag = inMinigameTag;
}

FGameplayTag UPDWMinigameFlowAsset::GetMinigameTag() const
{
	return Configuration.MiniGameTag;
}
