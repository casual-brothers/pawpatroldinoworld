// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWTransitionWidget.h"
#include "Managers/PDWEventSubsytem.h"

UPDWTransitionWidget::UPDWTransitionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
//#if WITH_EDITOR
//	Category = TEXT("Utility");
//	NodeDisplayStyle =FlowNodeStyle::InOut;
//#endif
}

void UPDWTransitionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TransitionIn)
	{
		TransitionInEvent.BindUFunction(this, FName("ManageTransitionInAnimationFinished"));
		BindToAnimationFinished(TransitionIn, TransitionInEvent);
	}
	if (TransitionOut)
	{
		TransitionOutEvent.BindUFunction(this, FName("ManageTransitionOutAnimationFinished"));
		BindToAnimationFinished(TransitionOut, TransitionOutEvent);
	}
}

void UPDWTransitionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bStartCount)
	{
		if (CurrentTime < MinTimeFromEndTransitionIn)
		{
			CurrentTime += InDeltaTime;
		}
		else
		{
			bStartCount = false;
			CurrentTime = 0.f;
			UPDWEventSubsytem::Get(this)->OnTransitionInEndEvent();
			StartTransitionOut();
		}
	}
}

void UPDWTransitionWidget::NativeDestruct()
{
	bIsScreenBlack = false;
	Super::NativeDestruct();
}

void UPDWTransitionWidget::StartTransitionIn()
{
	if (TransitionIn)
	{
		PlayAnimation(TransitionIn);
	}
}

void UPDWTransitionWidget::StartTransitionOut()
{
	bIsScreenBlack = false;
	if (IsAnimationPlaying(TransitionIn))
	{
		bStartTransitionOut = true;
	}
	else
	{
		if (TransitionOut)
		{
			PlayAnimation(TransitionOut);
		}
	}
}

void UPDWTransitionWidget::ManageTransitionInAnimationFinished()
{
	bIsScreenBlack = true;
	if (bStartTransitionOut)
	{
		bStartTransitionOut = false;
		bStartCount = true;
	}
	else
	{
		UPDWEventSubsytem::Get(this)->OnTransitionInEndEvent();
	}
}

void UPDWTransitionWidget::ManageTransitionOutAnimationFinished()
{
	UPDWEventSubsytem::Get(this)->OnTransitionOutEndEvent();
	RemoveFromParent();
}