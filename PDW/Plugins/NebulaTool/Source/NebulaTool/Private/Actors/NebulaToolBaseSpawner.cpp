#include "Actors/NebulaToolBaseSpawner.h"

#define LOCTEXT_NAMESPACE "NebulaToolBaseSpawner"

ANebulaToolBaseSpawner::ANebulaToolBaseSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PrePhysics;

	SetCanBeDamaged(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
}

void ANebulaToolBaseSpawner::Destroyed()
{
	if (!GetWorld()->IsGameWorld())
	{
		DestroySpawnedChildren();
	}

	Super::Destroyed();
}

#if WITH_EDITOR

void ANebulaToolBaseSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Spawn();
}

void ANebulaToolBaseSpawner::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	Spawn();
}

#endif

void ANebulaToolBaseSpawner::Spawn()
{
	DestroySpawnedChildren();
}

void ANebulaToolBaseSpawner::DestroySpawnedChildren()
{

}

#undef LOCTEXT_NAMESPACE