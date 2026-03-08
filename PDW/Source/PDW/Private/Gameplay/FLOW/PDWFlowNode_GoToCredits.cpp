// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_GoToCredits.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

UPDWFlowNode_GoToCredits::UPDWFlowNode_GoToCredits(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
	{
	#if WITH_EDITOR
		Category = TEXT("PDW|Quest");
		NodeDisplayStyle = FlowNodeStyle::Default;
	#endif
	}

void UPDWFlowNode_GoToCredits::ChangeState()
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetInGameCreditsTag().ToString(),"");
}