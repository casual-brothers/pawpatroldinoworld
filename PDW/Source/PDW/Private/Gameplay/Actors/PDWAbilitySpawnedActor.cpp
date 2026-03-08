// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWAbilitySpawnedActor.h"

APDWAbilitySpawnedActor::APDWAbilitySpawnedActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CustomRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CustomRootComponent"));
	SetRootComponent(CustomRootComponent);
}

void APDWAbilitySpawnedActor::SendAbilityInfo(const FAbilityInfo& inAbilityInfo)
{
	AbilityInfo = inAbilityInfo;
}

FAbilityInfo APDWAbilitySpawnedActor::GetAbilityInfo()
{
	return AbilityInfo;
}

void APDWAbilitySpawnedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APDWAbilitySpawnedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

