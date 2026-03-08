// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWLoadUserFSMState.h"
#include "Data/FlowDeveloperSettings.h"

void UPDWLoadUserFSMState::OnUserLoaded(int ControllerId)
{
	TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
}
