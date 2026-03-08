// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_CheckFact.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/PDWLocalPlayer.h"
#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

UPDWFlowNode_CheckFact::UPDWFlowNode_CheckFact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Fact");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif

	OutputPins.Empty();
	OutputPins.Add(FFlowPin(TEXT("True")));
	OutputPins.Add(FFlowPin(TEXT("False")));
}

void UPDWFlowNode_CheckFact::ExecuteInput(const FName& PinName)
{
	bool bValue = UPDWDataFunctionLibrary::GetFact(GetWorld(), Fact);
	if (bValue)
	{
		TriggerOutput(TEXT("True"), true);
		return;
	}
	TriggerOutput(TEXT("False"), true);
}

#if WITH_EDITOR
FString UPDWFlowNode_CheckFact::GetNodeDescription() const
{
	return GetIdentityTagDescription(Fact);
}
#endif