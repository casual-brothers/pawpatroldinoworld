// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWNavButtonWithParameter.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Core/NebulaFlowPlayerController.h"

void UPDWNavButtonWithParameter::OnButtonTriggered()
{
	BP_OnButtonTrigger();
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, NavbarButtonData.ButtonAction.ToString(), Param.ToString(), InputTriggerSender);
}