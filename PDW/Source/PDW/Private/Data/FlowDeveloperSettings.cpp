// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/FlowDeveloperSettings.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"

UE_DEFINE_GAMEPLAY_TAG(FlowFSMPages, "FSM.Pages");

UFlowDeveloperSettings::UFlowDeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UFlowDeveloperSettings* UFlowDeveloperSettings::Get()
{
	return CastChecked<UFlowDeveloperSettings>(UFlowDeveloperSettings::StaticClass()->GetDefaultObject());
}
