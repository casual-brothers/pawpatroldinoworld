// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_TriggerNebulaAction.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"

UPDWFlowNode_TriggerNebulaAction::UPDWFlowNode_TriggerNebulaAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|NebulaFSM");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_TriggerNebulaAction::ExecuteInput(const FName& PinName)
{
	if (bWaitForCallback)
	{
		UPDWEventSubsytem::Get(this)->BindToInteractionEvents(ActionToTrigger)->AddUniqueDynamic(this, &UPDWFlowNode_TriggerNebulaAction::OnCallbackTrigger);
	}
	UNebulaFlowCoreFunctionLibrary::TriggerAction(GetWorld(), ActionToTrigger.ToString(), "", nullptr);
	Super::ExecuteInput(PinName);
	if (!bWaitForCallback)
	{
		TriggerFirstOutput(true);
	}
}

void UPDWFlowNode_TriggerNebulaAction::OnCallbackTrigger(const FInteractionEventSignature& inEvent)
{
	if (UPDWEventSubsytem::Get(this)->BindToInteractionEvents(ActionToTrigger))
	{
		UPDWEventSubsytem::Get(this)->BindToInteractionEvents(ActionToTrigger)->RemoveDynamic(this, &UPDWFlowNode_TriggerNebulaAction::OnCallbackTrigger);
	}
	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FString UPDWFlowNode_TriggerNebulaAction::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s") , ActionToTrigger.IsValid() ? *ActionToTrigger.ToString() : TEXT("Invalid Tag"));
}
#endif