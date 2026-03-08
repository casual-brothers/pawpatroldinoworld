// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWProjectileActor.h"

APDWProjectileActor::APDWProjectileActor()
{
	ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComp"));
	SetRootComponent(ProjectileMeshComp);
}
