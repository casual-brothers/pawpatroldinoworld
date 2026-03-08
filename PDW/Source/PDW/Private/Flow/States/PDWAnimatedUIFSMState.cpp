// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Flow/States/PDWAnimatedUIFSMState.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"
#include "UI/Pages/PDWAnimatedUIPage.h"
#include "Data/FlowDeveloperSettings.h"

void UPDWAnimatedUIFSMState::OnFSMStateEnter_Implementation(const FString& InOption /* = FString("") */)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	FNavigationConfig& NavigationConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig.bTabNavigation = bIsEnableTabNavigation;

	if(UPDWAnimatedUIPage* AnimatedPage = Cast<UPDWAnimatedUIPage>(PageRef))
	{
		AnimatedPage->OnUIAnimationFinished.AddUniqueDynamic(this, &UPDWAnimatedUIFSMState::OnAnimationFinished);
	}
}

void UPDWAnimatedUIFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (bSkippable)
	{
		OnAnimationFinished();
	}
}

void UPDWAnimatedUIFSMState::OnAnimationFinished()
{
	TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
}
