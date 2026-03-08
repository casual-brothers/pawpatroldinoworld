// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWManageTransitionNode.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"
#include "Managers/PDWUIManager.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWManageTransitionNode::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	ManageTransition();
}

void UPDWManageTransitionNode::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		TriggerFirstOutput(true);
	}
	Super::OnPassThrough_Implementation();
}

void UPDWManageTransitionNode::ManageTransition()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (TransitionOption.IsNone() || (!UIManager))
	{
		return;
	}

	TArray<FName> Options = UPDWUIFunctionLibrary::GetTrasitionOption();
	if (TransitionOption == Options[0])
	{
		if (UIManager->IsTransitionOnWithScreenBlack())
		{
			OnTransitionEnd();
		}
		else
		{
			UIManager->StartTransitionIn();
			if (bContinueImmediately)
			{
				OnTransitionEnd();
			}
			else
			{
				UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::OnTransitionEnd);
			}
		}
	}
	else
	{
		UIManager->StartTransitionOut();
		if (bContinueImmediately)
		{
			OnTransitionEnd();
		}
		else
		{
			UPDWEventSubsytem::Get(this)->OnTransitionOutEnd.AddUniqueDynamic(this, &ThisClass::OnTransitionEnd);
		}
	}
}

void UPDWManageTransitionNode::OnTransitionEnd()
{
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::OnTransitionEnd);
	UPDWEventSubsytem::Get(this)->OnTransitionOutEnd.RemoveDynamic(this, &ThisClass::OnTransitionEnd);
	TriggerFirstOutput(true);
}

void UPDWManageTransitionNode::ExecuteInput(const FName& PinName)
{
	ManageTransition();
	Super::ExecuteInput(PinName);
}

#if WITH_EDITOR
FString UPDWManageTransitionNode::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s"), *TransitionOption.ToString());
}
#endif