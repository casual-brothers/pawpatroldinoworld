// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WPTeleportLocation.h"
#include "WPHelperSubsystem.h"

// Sets default values
AWPTeleportLocation::AWPTeleportLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportRoot"));
	SetRootComponent(SceneRoot);
	SceneRoot->SetComponentTickEnabled(false);
	PlayerTwoSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerTwoLocation"));
	PlayerTwoSpawnLocation->SetupAttachment(GetRootComponent());
	PlayerTwoSpawnLocation->SetComponentTickEnabled(false);

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
}

void AWPTeleportLocation::GetTeleportTransform(TArray<FTransform>& OutTransforms) const
{
	OutTransforms.Add(GetActorTransform());
	OutTransforms.Add(PlayerTwoSpawnLocation->GetComponentTransform());
}

// Called when the game starts or when spawned
void AWPTeleportLocation::BeginPlay()
{
	Super::BeginPlay();
	
	//REGISTER TELEPORT LOCATION TO THE SUBSYSTEM
	UWPHelperSubsystem* WPHelperSubsystem = UWPHelperSubsystem::Get(this);
	WPHelperSubsystem->RegisterTeleportLocation(TeleportID,this);
}

