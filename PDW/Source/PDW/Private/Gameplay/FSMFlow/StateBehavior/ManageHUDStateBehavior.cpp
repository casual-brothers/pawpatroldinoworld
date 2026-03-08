// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/StateBehavior/ManageHUDStateBehavior.h"
#include "Managers/PDWHUDSubsystem.h"

void UManageHUDStateBehavior::OnEnter_Implementation()
{
	UPDWHUDSubsystem::Get(this)->ManageHUD(HUDControl, HUDType);
}