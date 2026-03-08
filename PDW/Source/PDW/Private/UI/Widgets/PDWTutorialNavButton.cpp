// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWTutorialNavButton.h"

void UPDWTutorialNavButton::InitializeTutorialButton(const FPDWTutorialButtonData& NewTutorialButtonData)
{
	TutorialButtonData = NewTutorialButtonData;
	InitializeButton(NewTutorialButtonData.NavButtonData);
	if (TutorialButtonData.TriggerType == ETriggerInputType::OnTimePassed)
	{
		bStartCountForTimePassed = true;
	}
}

void UPDWTutorialNavButton::OnButtonTriggered()
{
	Super::OnButtonTriggered();
	switch (TutorialButtonData.TriggerType)
	{
	case ETriggerInputType::OnInputPressed:
		TriggerCallback();
		break;
	case ETriggerInputType::OnInputHoldPressed:
		if (CurrentTriggerPassedTime >= TutorialButtonData.TimeInputHold)
		{
			TriggerCallback();
		}
		break;
	}
}

void UPDWTutorialNavButton::OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	Super::OnInputTriggered(Instance, inPC);
	CurrentTriggerPassedTime = Instance.GetElapsedTime();
}

void UPDWTutorialNavButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bStartCountForTimePassed)
	{
		CurrentTimePassed += InDeltaTime;
		if (CurrentTimePassed >= TutorialButtonData.TimeToWait)
		{
			TriggerCallback();
			bStartCountForTimePassed = false;
		}
	}
}

void UPDWTutorialNavButton::TriggerCallback()
{
	if (!bCallBackCalled && TutorialButtonData.CallBack)
	{
		bCallBackCalled = true;
		TutorialButtonData.CallBack();
	}
}