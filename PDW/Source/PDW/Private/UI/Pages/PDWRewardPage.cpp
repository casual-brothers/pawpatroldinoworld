// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Pages/PDWRewardPage.h"
#include "Data/PDWGameplayStructures.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "Data/PDWGameSettings.h"
#include "UI/Widgets/PDWUnlockWidget.h"

void UPDWRewardPage::InitRewardPage(FPDWUnlockableContainer& InRewardsToUnlock, bool bIsFromLevelUp)
{
	if (!InRewardsToUnlock.Unlocakbles.Num())
	{
		return;
	}
	RewardsToUnlock = InRewardsToUnlock;
	CurrentIndex = 0;
	if (bIsFromLevelUp && OnLevelUp)
	{
		PlayAnimation(OnLevelUp);
	}
	else
	{
		ShowCurrentUnlockable();
	}
}

void UPDWRewardPage::ShowCurrentUnlockable()
{
	PageNavbar->DisableButton(ConfirmNavButtonID);
	if (RewardsToUnlock.Unlocakbles[CurrentIndex]->UnlockableType == EUnlockableType::PupCustomizable)
	{
		PageNavbar->AddNavbarButton(CustomizeNavButtonID, ENavElementPosition::RIGHT);
		PageNavbar->DisableButton(CustomizeNavButtonID);
		PageNavbar->AddNavbarButton(EquipNowButtonID, ENavElementPosition::RIGHT);
		PageNavbar->DisableButton(EquipNowButtonID);
	}
	else
	{
		PageNavbar->RemoveNavbarButton(CustomizeNavButtonID);
	}

	BP_ShowCurrentUnlockable(RewardsToUnlock.Unlocakbles[CurrentIndex]);
	UnlockWidget->InitUnlockWidget(RewardsToUnlock.Unlocakbles[CurrentIndex]);
	UnlockWidget->OnEndShowAnimationFinished.AddUniqueDynamic(this, &ThisClass::ManageOnEndShowAnimationFinished);
}

void UPDWRewardPage::ManageOnEndShowAnimationFinished()
{
	PageNavbar->EnableButton(ConfirmNavButtonID);
	PageNavbar->EnableButton(CustomizeNavButtonID);
	PageNavbar->EnableButton(EquipNowButtonID);
}

void UPDWRewardPage::NativeDestruct()
{
	UnlockWidget->OnEndShowAnimationFinished.RemoveDynamic(this, &ThisClass::ManageOnEndShowAnimationFinished);
	Super::NativeDestruct();
}

void UPDWRewardPage::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	if (Animation == OnLevelUp)
	{
		ShowCurrentUnlockable();
	}
}

bool UPDWRewardPage::CheckForNextUnlockable()
{
	CurrentIndex++;
	if (RewardsToUnlock.Unlocakbles.IsValidIndex(CurrentIndex))
	{
		ShowCurrentUnlockable();
		return true;
	}
	else
	{
		return false;
	}
}