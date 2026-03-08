// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWBabyPaleoDinoComponent.h"

// Sets default values for this component's properties
UPDWBabyPaleoDinoComponent::UPDWBabyPaleoDinoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UPDWBabyPaleoDinoComponent::Init(FGameplayTag _PenTag, APDWDinoPenArea* _PenArea)
{
	PenTag = _PenTag;
	PenArea = _PenArea;

	OnInit.Broadcast();
}

// Called when the game starts
void UPDWBabyPaleoDinoComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
