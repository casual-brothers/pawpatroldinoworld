// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWPaleoCenterCustomization.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FlowComponent.h"

// Sets default values
APDWPaleoCenterCustomization::APDWPaleoCenterCustomization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FlowComponent = CreateDefaultSubobject<UFlowComponent>(TEXT("FlowComponent"));
}

void APDWPaleoCenterCustomization::ChangeCustomizationVisibility(bool IsVisible)
{
	SetActorHiddenInGame(!IsVisible);
	SetActorEnableCollision(IsVisible);
}

// Called when the game starts or when spawned
void APDWPaleoCenterCustomization::BeginPlay()
{
	Super::BeginPlay();

	if (FlowComponent)
	{
		int32 QuantityInInventory = UPDWDataFunctionLibrary::GetItemQuantityFromInventory(this, FlowComponent->IdentityTags.First());
		ChangeCustomizationVisibility(QuantityInInventory > 0);
	}
}

