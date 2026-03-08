// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWCollectibleBase.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/Components/PDWPickUpComponent.h"
#include "Managers/PDWEventSubsytem.h"
#include "FlowComponent.h"

// Sets default values
APDWCollectibleBase::APDWCollectibleBase()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CollectibleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollectibleMesh"));
	CollectibleMeshComponent->SetGenerateOverlapEvents(false);
	CollectibleMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	CollectibleMeshComponent->SetupAttachment(RootComponent);
	PickUpComponent = CreateDefaultSubobject<UPDWPickUpComponent>(TEXT("PickUpComponent"));
	PickUpComponent->SetupAttachment(RootComponent);
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APDWCollectibleBase::BeginPlay()
{
	Super::BeginPlay();
	
	RootComponent->SetMobility(EComponentMobility::Movable); //To avoid static root component bug

	if (PickUpComponent)
	{
		PickUpComponent->OnPickUpStart.AddUniqueDynamic(this, &APDWCollectibleBase::OnPickUpStart);
		PickUpComponent->OnPickUpEnd.AddUniqueDynamic(this, &APDWCollectibleBase::OnPickUpEnd);
	}
}

void APDWCollectibleBase::OnPickUpStart(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	if (PickUpComponent)
	{
		PickUpComponent->OnPickUpStart.RemoveDynamic(this, &APDWCollectibleBase::OnPickUpStart);
	}
}

void APDWCollectibleBase::OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	if (PickUpComponent)
	{
		PickUpComponent->OnPickUpEnd.RemoveDynamic(this, &APDWCollectibleBase::OnPickUpEnd);
	}

	BP_PickUpEnd();
	Destroy();
}

