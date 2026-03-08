// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Gameplay/FLOW/UPDWFlowNode_ToggleInputActionAvailability.h"
#include "InputAction.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"


UUPDWFlowNode_ToggleInputActionAvailability::UUPDWFlowNode_ToggleInputActionAvailability(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Utility");
	NodeDisplayStyle =FlowNodeStyle::InOut;
#endif
}


void UUPDWFlowNode_ToggleInputActionAvailability::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	Implementation();
}

void UUPDWFlowNode_ToggleInputActionAvailability::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	Implementation();
}

void UUPDWFlowNode_ToggleInputActionAvailability::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		Implementation();
	}

	Super::OnPassThrough_Implementation();
}


void UUPDWFlowNode_ToggleInputActionAvailability::Implementation()
{
	if (bEnable)
	{	
		UPDWGameplayFunctionLibrary::RemoveDisabledInputAction(this, InputAction.Get()->GetFName());
	}
	else
	{	
		UPDWGameplayFunctionLibrary::AddDisabledInputAction(this, InputAction.Get()->GetFName());
	}

	TriggerFirstOutput(true);
}
