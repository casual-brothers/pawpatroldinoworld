// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PDWGameplayTagsFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"

FGameplayTag UPDWGameplayTagsFunctionLibrary::GetRootTagFromTag(const FGameplayTag& inTag)
{
	FGameplayTag RootTag = FGameplayTag::EmptyTag;

	if (inTag.MatchesTag(UPDWGameplayTagSettings::GetInteractionActionTag()))
	{
		RootTag = UPDWGameplayTagSettings::GetInteractionActionTag();
	}
	else if (inTag.MatchesTag(UPDWGameplayTagSettings::GetInteractionSkillTag()))
	{
		RootTag = UPDWGameplayTagSettings::GetInteractionSkillTag();
	}
	else if (inTag.MatchesTag(UPDWGameplayTagSettings::GetInteractionAreaTag()))
	{
		RootTag = UPDWGameplayTagSettings::GetInteractionAreaTag();
	}
	else if (inTag.MatchesTag(UPDWGameplayTagSettings::GetMinigameInteractionTag()))
	{
		RootTag = UPDWGameplayTagSettings::GetMinigameInteractionTag();
	}
	return RootTag;
}

