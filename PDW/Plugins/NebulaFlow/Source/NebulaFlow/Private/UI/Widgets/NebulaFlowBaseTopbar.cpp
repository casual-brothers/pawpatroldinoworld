// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/NebulaFlowBaseTopbar.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Components/TextBlock.h"
#include "UObject/UObjectGlobals.h"
#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "Components/PanelSlot.h"
#include "Components/OverlaySlot.h"

UNebulaFlowBaseTopbar::UNebulaFlowBaseTopbar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNebulaFlowBaseTopbar::SetTitle(FText InTitle)
{
	if (bHideLabelWhenNavigationEnabled && bEnableNavigation)
		return;

	TopbarTitle->SetText(InTitle);
	TopbarTitle->SetVisibility(ESlateVisibility::Visible);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UNebulaFlowBaseTopbar::SetTitlePosition(ENavElementPosition InPosition)
{
	EHorizontalAlignment newAlignment = HAlign_Center;

	if (InPosition == ENavElementPosition::LEFT)
		newAlignment = HAlign_Left;

	if (InPosition == ENavElementPosition::RIGHT)
		newAlignment = HAlign_Right;
		
	Cast<UOverlaySlot>(TopbarTitle->Slot)->SetHorizontalAlignment(newAlignment);
}

void UNebulaFlowBaseTopbar::ShowUserName(bool bShow)
{
	ensure(TopbarUserLabel);
	if (TopbarUserLabel == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Topbar request to show username without label"));
		return;
	}
	
	if (bShow)
	{
		UNebulaFlowLocalPlayer* Player = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
		if (Player)
		{
			TopbarUserLabel->SetText(FText::FromString(Player->GetNickname()));
			TopbarUserLabel->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		TopbarUserLabel->SetVisibility(ESlateVisibility::Collapsed);
	}

}

int32 UNebulaFlowBaseTopbar::NavigateTo(int32 InDestination /*+1 or -1 to indicate left-right*/)
{
	return 0;
}

void UNebulaFlowBaseTopbar::InitializeTopBar()
{
	
}