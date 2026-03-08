// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_TogglePenActivity.h"
#include "Managers/PDWPaleoCenterSubsystem.h"

UPDWFlowNode_TogglePenActivity::UPDWFlowNode_TogglePenActivity(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|PaleoCenter");
#endif

}

void UPDWFlowNode_TogglePenActivity::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->ChangeDinoPenActivity(DinoPenTag, IsActive, ResetNeedTimer);
	}

	TriggerFirstOutput(true);
}

void UPDWFlowNode_TogglePenActivity::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
		{
			PaleoCenterSubsystem->ChangeDinoPenActivity(DinoPenTag, IsActive, ResetNeedTimer);
		}
	}

	Super::OnPassThrough_Implementation();
}
