// Fill out your copyright notice in the Description page of Project Settings.

#include "Flow/States/PDWCreditsFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Data/PDWGameSettings.h"
#include "UI/Pages/PDWCreditsPage.h"

void UPDWCreditsFSMState::OnFSMStateEnter_Implementation(const FString& InOption /* = FString("") */)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UPDWCreditsPage* CreditsPage = Cast<UPDWCreditsPage>(PageRef);
	if (!ensureMsgf(CreditsPage, TEXT("Credits page is of the wrong type!")))
	{
		return;
	}

	// if loop scrolling is set to false, credits will exit once they reach the end of the scrolling
	// otherwise player needs to manually press back (default)
	if (CreditsPage->GetLoopScrolling() == false)
	{
		CreditsPage->OnCreditsEndReached.AddUniqueDynamic(this, &UPDWCreditsFSMState::OnCreditsEndReached);
	}
}

void UPDWCreditsFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UPDWGameSettings::GetUIActionBack())
	{
		TriggerTransition(UFlowDeveloperSettings::GetBackTag().GetTagName());
	}
}

void UPDWCreditsFSMState::OnFSMStateExit_Implementation()
{
	Cast<UPDWCreditsPage>(PageRef)->OnCreditsEndReached.RemoveAll(this);

	Super::OnFSMStateExit_Implementation();
}

void UPDWCreditsFSMState::OnCreditsEndReached()
{
	TriggerTransition(UFlowDeveloperSettings::GetBackTag().GetTagName());
}