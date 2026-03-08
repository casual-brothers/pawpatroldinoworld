// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWWaterStationHandler.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

// Sets default values for this component's properties
UPDWWaterStationHandler::UPDWWaterStationHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPDWWaterStationHandler::InitWaterStation(FGameplayTag Tag)
{
	DinoPenTag = Tag;
	OnInit.Broadcast();
}

float UPDWWaterStationHandler::GetCurrentWater()
{
	return UPDWDataFunctionLibrary::GetStationWater(this, DinoPenTag);
}

void UPDWWaterStationHandler::SetCurrentWater(float NewValue)
{
	UPDWDataFunctionLibrary::UpdateWaterInStation(this, DinoPenTag, NewValue);
}

void UPDWWaterStationHandler::HandleEndInteraction()
{
	OnSmartObjectUpdate.Broadcast(true);
}

