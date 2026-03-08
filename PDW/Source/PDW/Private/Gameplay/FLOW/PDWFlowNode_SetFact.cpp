// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_SetFact.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/PDWLocalPlayer.h"
#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

UPDWFlowNode_SetFact::UPDWFlowNode_SetFact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Fact");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_SetFact::ExecuteInput(const FName& PinName)
{
	UPDWDataFunctionLibrary::SetFact(GetWorld(), Fact, bFactValue);
	TriggerFirstOutput(true);
	Super::ExecuteInput(PinName);
}

#if WITH_EDITOR
FString UPDWFlowNode_SetFact::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s   %s"), *GetIdentityTagDescription(Fact), bFactValue ? TEXT("true") : TEXT("false"));
}
#endif