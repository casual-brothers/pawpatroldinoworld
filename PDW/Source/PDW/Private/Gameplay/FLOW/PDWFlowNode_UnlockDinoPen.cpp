// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_UnlockDinoPen.h"
#include "Managers/PDWPaleoCenterSubsystem.h"

UPDWFlowNode_UnlockDinoPen::UPDWFlowNode_UnlockDinoPen(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
	{
		#if WITH_EDITOR
		Category = TEXT("PDW|Quest");
		NodeDisplayStyle = FlowNodeStyle::Default;
		#endif
	}

void UPDWFlowNode_UnlockDinoPen::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	UnlockDinoPen();
	TriggerFirstOutput(true);
}

void UPDWFlowNode_UnlockDinoPen::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		UnlockDinoPen();
	}

	Super::OnPassThrough_Implementation();
}

void UPDWFlowNode_UnlockDinoPen::UnlockDinoPen()
{
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->UnlockDinoPen(DinoPenTag, StartInactive);
	}
}

#if WITH_EDITOR
FString UPDWFlowNode_UnlockDinoPen::GetNodeDescription() const
{
	return DinoPenTag.ToString();
}
#endif
