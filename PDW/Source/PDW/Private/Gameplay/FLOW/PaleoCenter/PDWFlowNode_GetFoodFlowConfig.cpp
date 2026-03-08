// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWFlowNode_GetFoodFlowConfig.h"
#include "Gameplay/FLOW/PaleoCenter/PDWFoodFlow.h"

void UPDWFlowNode_GetFoodFlowConfig::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	TriggerFirstOutput(true);
}

void UPDWFlowNode_GetFoodFlowConfig::InitializeInstance()
{
	InitOutputPins();
	Super::InitializeInstance();
}

void UPDWFlowNode_GetFoodFlowConfig::InitOutputPins()
{
	UPDWFoodFlow* Asset = Cast<UPDWFoodFlow>(GetFlowAsset());
	FPDWFoodFlowConfig Config = Asset->GetFoodFlowConfig();
	OutFoodPlantTag.Value = Config.FoodPlantTag;
	OutFoodStationTag.Value = Config.FoodStationTag;
	OutFoodTypeTag.Value = Config.FoodTypeTag;
}
