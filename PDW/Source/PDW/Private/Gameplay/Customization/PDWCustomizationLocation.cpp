// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Customization/PDWCustomizationLocation.h"
#include "Camera/CameraComponent.h"

APDWCustomizationLocation::APDWCustomizationLocation()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CustomizationCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CustomizationCamera"));
	CustomizationCamera->SetupAttachment(RootComponent);
}

void APDWCustomizationLocation::BeginPlay()
{
	Super::BeginPlay();

}