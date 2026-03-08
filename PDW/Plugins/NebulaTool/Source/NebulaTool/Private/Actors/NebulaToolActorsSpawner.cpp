#include "Actors/NebulaToolActorsSpawner.h"

#include "NebulaTool.h"
#include "UObject/NoExportTypes.h"

#define LOCTEXT_NAMESPACE "NebulaToolActorsSpawner"

void ANebulaToolActorsSpawner::Spawn()
{
	Super::Spawn();

	if (bSnapToGround)
	{
		FVector StartPoint = GetActorLocation();
		FVector EndPoint = StartPoint + FVector(0.0f, 0.0f, -100000.0f);

		FHitResult HitResult = {};

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);

		GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, GroundCollisionChannel, CollisionQueryParams);
		if (HitResult.bBlockingHit)
		{
			SetActorLocation(HitResult.Location);

			for (int32 SplinePointIndex = 0; SplinePointIndex < SplineComponent->GetNumberOfSplinePoints(); ++SplinePointIndex)
			{
				FVector Location = SplineComponent->GetLocationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
				SplineComponent->SetLocationAtSplinePoint(SplinePointIndex, FVector(Location.X, Location.Y, HitResult.Location.Z), ESplineCoordinateSpace::Local);
			}
		}
	}

	if (SpawnType == ENebulaToolActorsSpawnerSpawnType::OnSpline)
	{
		for (int32 ActorIndex = 0; ActorIndex < ActorsToSpawn.Default; ++ActorIndex)
		{
			// Spawn actor
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.OverrideLevel = GetLevel();

			AActor* ActorToSpawn = GetWorld()->SpawnActor<AActor>(ActorToSpawnClass, ActorSpawnParameters);
			if (ActorToSpawn)
			{
				ActorToSpawn->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

				const float SplineLength = SplineComponent->GetSplineLength();

				// Actor distance from spline start
				const float ActorDistanceAlongSpline = SplineLength * ActorIndex / (ActorsToSpawn.Default - 1);

				// Actor location on spline
				const FVector ActorLocation = SplineComponent->GetLocationAtDistanceAlongSpline(ActorDistanceAlongSpline, ESplineCoordinateSpace::Local);
				ActorToSpawn->SetActorRelativeLocation(ActorLocation);
			}
			else
			{
				UE_LOG(LogNebulaTool, Warning, TEXT("Actor not spawned."));
			}
		}
	}
	else
	{
		for (int32 ActorIndexOnX = 0; ActorIndexOnX < ActorsToSpawnOnX.Default; ++ActorIndexOnX)
		{
			for (int32 ActorIndexOnY = 0; ActorIndexOnY < ActorsToSpawnOnY.Default; ++ActorIndexOnY)
			{
				// Spawn actor
				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.OverrideLevel = GetLevel();

				AActor* ActorToSpawn = GetWorld()->SpawnActor<AActor>(ActorToSpawnClass, ActorSpawnParameters);
				if (ActorToSpawn)
				{
					ActorToSpawn->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

					// Actor location on grid
					const FVector ActorLocation = FVector((DistanceOnX * ActorIndexOnX), (DistanceOnY * ActorIndexOnY), 0.0f);
					ActorToSpawn->SetActorRelativeLocation(ActorLocation);
				}
				else
				{
					UE_LOG(LogNebulaTool, Warning, TEXT("Actor not spawned."));
				}
			}
		}
	}
}

void ANebulaToolActorsSpawner::DestroySpawnedChildren()
{
	TArray<AActor*> SpawnedActorList;
	GetAttachedActors(SpawnedActorList);
	for (AActor* SpawnedActor : SpawnedActorList)
	{
		if (SpawnedActor)
		{
			SpawnedActor->Destroy();
			SpawnedActor = nullptr;
		}
	}

	Super::DestroySpawnedChildren();
}

#undef LOCTEXT_NAMESPACE