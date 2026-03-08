// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWFlowNode_IsPenActive.h"
#include "Nodes/FlowNodeBase.h"
#include "Managers/PDWPaleoCenterSubsystem.h"

const FName UPDWFlowNode_IsPenActive::INPIN_Evaluate = TEXT("Evaluate");
const FName UPDWFlowNode_IsPenActive::OUTPIN_True = TEXT("True");
const FName UPDWFlowNode_IsPenActive::OUTPIN_False = TEXT("False");

UPDWFlowNode_IsPenActive::UPDWFlowNode_IsPenActive(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|PaleoCenter");
	NodeDisplayStyle = FlowNodeStyle::Logic;
#endif

	InputPins.Empty();
	InputPins.Add(FFlowPin(INPIN_Evaluate));
	InputPins.Add(FFlowPin(TEXT("DinoPenTag"), EFlowPinType::GameplayTag));

	OutputPins.Empty();
	OutputPins.Add(FFlowPin(OUTPIN_True));
	OutputPins.Add(FFlowPin(OUTPIN_False));
}

void UPDWFlowNode_IsPenActive::ExecuteInput(const FName& PinName)
{
	bool Result = false;
	FGameplayTag PenTag = UFlowNodeBase::TryResolveDataPinAsGameplayTag("DinoPenTag").Value;

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		Result = PaleoCenterSubsystem->GetDinoPenActivity(PenTag);
	}

	TriggerOutput(Result ? OUTPIN_True : OUTPIN_False, true);
}