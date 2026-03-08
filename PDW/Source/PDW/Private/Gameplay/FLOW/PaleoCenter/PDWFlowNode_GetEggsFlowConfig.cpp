// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWFlowNode_GetEggsFlowConfig.h"
#include "Gameplay/FLOW/PaleoCenter/PDWEggsFlow.h"

void UPDWFlowNode_GetEggsFlowConfig::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	TriggerFirstOutput(true);
}

void UPDWFlowNode_GetEggsFlowConfig::InitializeInstance()
{	
	InitOutputPins();
	Super::InitializeInstance();
}

void UPDWFlowNode_GetEggsFlowConfig::InitOutputPins()
{
	UPDWEggsFlow* Asset = Cast<UPDWEggsFlow>(GetFlowAsset());
	FPDWEggFlowConfig Config = Asset->GetEggFlowConfig();
	OutEggsTag.Value = Config.EggTag;
	OutNestTag.Value = Config.NestTag;
	OutPenTag.Value = Config.PenTag;
	OutNestTeleportTag.Value = Config.OutNestTeleportTag;
	//OutNestTeleportActor.SetObjectValue(Config.OutNestTeleportActor);
}
