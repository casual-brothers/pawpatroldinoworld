// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_ChangeState.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "FlowAsset.h"
#include "Modes/PDWGameplayGameMode.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayStructures.h"

void UPDWFlowNode_ChangeState::ContinueToNextNode()
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();	
	if(!EventSubsystem)
		return;

	EventSubsystem->OnSwitcherStateWaitForAction.RemoveDynamic(this, &UPDWFlowNode_ChangeState::ContinueToNextNode);

	TriggerFirstOutput(true);
}

void UPDWFlowNode_ChangeState::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	BindEvents();
	ChangeState();
}

void UPDWFlowNode_ChangeState::BindEvents()
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();	
	if(!EventSubsystem)
		return;

	EventSubsystem->OnSwitcherStateWaitForAction.AddUniqueDynamic(this, &UPDWFlowNode_ChangeState::ContinueToNextNode);
}

void UPDWFlowNode_ChangeState::ChangeState()
{

}
