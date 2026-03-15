// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Pages/PDWAnimatedUIPage.h"

void UPDWAnimatedUIPage::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	WidgetAnimationFinished.BindUFunction(this, FName("AnimationFinishedEvent"));
	this->BindToAnimationFinished(WidgetAnimation, WidgetAnimationFinished);
	PlayAnimation(WidgetAnimation);
}

void UPDWAnimatedUIPage::NativeDestruct()
{
	Super::NativeDestruct();
}


FReply UPDWAnimatedUIPage::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	KeyDown();

	return FReply::Handled();
}

void UPDWAnimatedUIPage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// User Focus Must always be on this page
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!this->HasAnyUserFocus())
	{
		FSlateApplication::Get().SetAllUserFocus(this->TakeWidget());
	}
}

void UPDWAnimatedUIPage::KeyDown()
{
	TriggerAction("KeyDown", FString(""));
}

void UPDWAnimatedUIPage::AnimationFinishedEvent()
{
	OnUIAnimationFinished.Broadcast();
}

