// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWUnlockWidget.h"
#include "GameplayTagContainer.h"

void UPDWUnlockWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Show)
	{
		OnShowAnimationFinished.BindUFunction(this, FName("ManageOnShowAnimationFinished"));
		BindToAnimationFinished(Show, OnShowAnimationFinished);
	}
}

void UPDWUnlockWidget::ManageOnShowAnimationFinished()
{
	OnEndShowAnimationFinished.Broadcast();
}

TArray<FGameplayTag> UPDWUnlockWidget::CheckForDinoIDParents(FGameplayTag inTag)
{
	TArray<FGameplayTag> UniqueParentTags;
	inTag.ParseParentTags(UniqueParentTags);
	return UniqueParentTags;
}
