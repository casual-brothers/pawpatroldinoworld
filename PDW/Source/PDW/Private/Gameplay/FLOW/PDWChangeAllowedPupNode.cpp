// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWChangeAllowedPupNode.h"
#include "Managers/PDWUIManager.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

UPDWChangeAllowedPupNode::UPDWChangeAllowedPupNode(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
}

void UPDWChangeAllowedPupNode::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	ChangeAllowedPup();
}

void UPDWChangeAllowedPupNode::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	ChangeAllowedPup();
	TriggerFirstOutput(true);
}

void UPDWChangeAllowedPupNode::ChangeAllowedPup()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (!UIManager)
	{
		return;
	}

	if (bResetAllowedPupsToDefault)
	{
		UIManager->ResetAllowedPups(true);
	}
	else
	{
		UIManager->ChangeAllowedPups(NewAllowedPups, NewAllowedPups,true);
	}
}

#if WITH_EDITOR
FString UPDWChangeAllowedPupNode::GetNodeDescription() const
{
	FString Result;
	if (bResetAllowedPupsToDefault)
	{
		Result = "Reset Pups To Default";
	}
	else
	{
		for (FGameplayTag PupTag : NewAllowedPups)
		{
			Result.Append(PupTag.ToString() + LINE_TERMINATOR);
		}
	}

	return Result;
}
#endif