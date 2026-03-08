// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Pages/PDWCreditsPage.h"

#include "Components/ScrollBox.h"

void UPDWCreditsPage::NativeConstruct()
{
	Super::NativeConstruct();

	CreditsScroller->SetScrollOffset(0.0f);
}

void UPDWCreditsPage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bScrollingEnded)
	{
		return;
	}

	CreditsScroller->SetScrollOffset(CreditsScroller->GetScrollOffset() + ScrollSpeed * InDeltaTime);

	if (CreditsScroller->GetScrollOffset() >= CreditsScroller->GetScrollOffsetOfEnd())
	{
		if (bLoopScrolling)
		{
			CreditsScroller->SetScrollOffset(0.0f);
		}
		else
		{
			bScrollingEnded = true;
			OnCreditsEndReached.Broadcast();
		}
	}
}
