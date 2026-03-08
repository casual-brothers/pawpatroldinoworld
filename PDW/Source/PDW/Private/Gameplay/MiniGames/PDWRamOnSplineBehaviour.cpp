// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWRamOnSplineBehaviour.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWRamOnSplineBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	if (!bRamMode)
	{
		Super::ExecuteBehaviour(inInputInstance);
		bRamMode = true;
		if (GetControllerOwner())
		{
			GetControllerOwner()->GetWorld()->GetTimerManager().SetTimer(RamModeTimerHandle, [=, this]() {
				bRamMode = false;
				UPDWEventSubsytem::Get(this)->OnStopRamEvent();
				BP_OnRamModeStop();
				},RamDuration,false);
			BP_OnRamModeStart();
		}
		UPDWEventSubsytem::Get(this)->OnRamModeEvent();
	}

}

void UPDWRamOnSplineBehaviour::UninitializeBehaviour()
{
	if (GetControllerOwner())
	{
		GetControllerOwner()->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	UPDWEventSubsytem::Get(this)->OnStopRamEvent();
	BP_OnRamModeStop();
	Super::UninitializeBehaviour();
}
