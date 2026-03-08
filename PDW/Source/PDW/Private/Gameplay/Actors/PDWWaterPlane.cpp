// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWWaterPlane.h"

APDWWaterPlane::APDWWaterPlane()
{
	PrimaryActorTick.bCanEverTick = false;

	WaterPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterPlaneComponent"));
	WaterPlaneComponent->SetupAttachment(RootComponent);
	SwimCollisionPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwimCollisionPlaneComponent"));
	SwimCollisionPlaneComponent->SetupAttachment(WaterPlaneComponent);
	DiveCollisionPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiveCollisionPlaneComponent"));
	DiveCollisionPlaneComponent->SetupAttachment(WaterPlaneComponent);
}





