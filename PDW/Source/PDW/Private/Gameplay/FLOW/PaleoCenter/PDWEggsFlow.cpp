// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWEggsFlow.h"

void UPDWEggsFlow::SetEggFlowConfig(FPDWEggFlowConfig EggFlowConfig)
{
	EggFlowConfiguration = EggFlowConfig;
}

FPDWEggFlowConfig& UPDWEggsFlow::GetEggFlowConfig()
{
	return EggFlowConfiguration;
}
