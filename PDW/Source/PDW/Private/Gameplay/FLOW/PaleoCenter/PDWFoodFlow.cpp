// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWFoodFlow.h"

void UPDWFoodFlow::SetFoodFlowConfig(FPDWFoodFlowConfig FoodFlowConfig)
{
	FoodFlowConfiguration = FoodFlowConfig;
}

FPDWFoodFlowConfig& UPDWFoodFlow::GetFoodFlowConfig()
{
	return FoodFlowConfiguration;
}
