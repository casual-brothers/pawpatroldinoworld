// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWAimingTargetActor.h"

// Sets default values
APDWAimingTargetActor::APDWAimingTargetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void APDWAimingTargetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APDWAimingTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

