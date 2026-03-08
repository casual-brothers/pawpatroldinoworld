// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWEndCurrentTutorialNavButton.h"
#include "Managers/PDWHUDSubsystem.h"

void UPDWEndCurrentTutorialNavButton::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	UPDWHUDSubsystem::Get(this)->EndCurrentHUDDialogue();
	UPDWHUDSubsystem::Get(this)->EndCurrentTutorialInputAction();
	TriggerFirstOutput(true);
}

void UPDWEndCurrentTutorialNavButton::OnPassThrough_Implementation()
{
	UPDWHUDSubsystem::Get(this)->EndCurrentHUDDialogue();
	UPDWHUDSubsystem::Get(this)->EndCurrentTutorialInputAction();
	Super::OnPassThrough_Implementation();
}