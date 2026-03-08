// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWFoodStationHandler.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

// Sets default values for this component's properties
UPDWFoodStationHandler::UPDWFoodStationHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UPDWFoodStationHandler::InitFoodStation(FGameplayTag _DinoPenTag, FGameplayTag _FoodTypeTag)
{
	DinoPenTag = _DinoPenTag;
	FoodTypeTag = _FoodTypeTag;
	OnInit.Broadcast();
}

int32 UPDWFoodStationHandler::GetCurrentStationFood()
{
	return UPDWDataFunctionLibrary::GetStationFood(this, DinoPenTag);
}

void UPDWFoodStationHandler::SetCurrentStationFood(float NewValue)
{
	UPDWDataFunctionLibrary::UpdateFoodInStation(this, DinoPenTag, NewValue);
}

void UPDWFoodStationHandler::HandleMinigameEnd()
{
	SetCurrentStationFood(1);
	OnSmartObjectUpdate.Broadcast(true);
}

