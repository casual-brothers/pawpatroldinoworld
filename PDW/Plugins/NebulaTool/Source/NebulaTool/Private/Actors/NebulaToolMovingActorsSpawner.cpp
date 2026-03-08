#include "Actors/NebulaToolMovingActorsSpawner.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NebulaTool.h"

#define LOCTEXT_NAMESPACE "NebulaToolMovingActorsSpawner"

ANebulaToolMovingActorsSpawner::ANebulaToolMovingActorsSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ANebulaToolMovingActorsSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetSpawnedActorList(RuntimeSpawnedActorList);
}

void ANebulaToolMovingActorsSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MovingActorsSpawnerSetupList.Num() > 0 && ActorsSpeed > 0.0f)
	{
		const float SplineLength = SplineComponent->GetSplineLength();
	
		for (int32 ActorIndex = 0; ActorIndex < RuntimeSpawnedActorList.Num(); ++ActorIndex)
		{
			AActor* SpawnedActor = RuntimeSpawnedActorList[ActorIndex];
			if (SpawnedActor && ActorDistanceAlongSplineList.IsValidIndex(ActorIndex))
			{
				// Actor distance along spline
				float ActorDistanceAlongSpline = ActorDistanceAlongSplineList[ActorIndex];
				if (bMoveActorsForward)
				{
					ActorDistanceAlongSpline += (ActorsSpeed * DeltaTime);

					if (ActorDistanceAlongSpline > SplineLength)
					{
						ActorDistanceAlongSpline = 0.0f;
					}
				}
				else
				{
					ActorDistanceAlongSpline -= (ActorsSpeed * DeltaTime);
			
					if (ActorDistanceAlongSpline < 0.0f)
					{
						ActorDistanceAlongSpline = SplineLength;
					}
				}
		
				ActorDistanceAlongSplineList[ActorIndex] = ActorDistanceAlongSpline;

				// Update actor transform on spline
				SpawnedActor->SetActorTransform(SplineComponent->GetTransformAtDistanceAlongSpline(ActorDistanceAlongSpline, ESplineCoordinateSpace::World));
			}
		}
	}
}

void ANebulaToolMovingActorsSpawner::SetActorsSpeed(const float InActorsSpeed)
{
	ActorsSpeed = InActorsSpeed;
}

void ANebulaToolMovingActorsSpawner::SetMoveActorsForward(const bool bInMoveActorsForward)
{
	bMoveActorsForward = bInMoveActorsForward;
}

float ANebulaToolMovingActorsSpawner::GetActorsSpeed() const
{
	return ActorsSpeed;
}

bool ANebulaToolMovingActorsSpawner::GetMoveActorsForward() const
{
	return bMoveActorsForward;
}

void ANebulaToolMovingActorsSpawner::GetSpawnedActorList(TArray<AActor*>& OutSpawnedActorList) const
{
	GetAttachedActors(OutSpawnedActorList);
}

void ANebulaToolMovingActorsSpawner::Spawn()
{
	Super::Spawn();

	SplineComponent->SetClosedLoop(bCloseLoop);

	TArray<AActor*>	SpawnedActorList = {};
	GetSpawnedActorList(SpawnedActorList);

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

	if (MovingActorsSpawnerSetupList.Num() > 0)
	{
		// Actors to spawn
		int32 ActorsToSpawnNumber = 0;
		for (const FNebulaToolMovingActorsSpawnerSetup& MovingActorsSpawnerSetup : MovingActorsSpawnerSetupList)
		{
			ActorsToSpawnNumber += MovingActorsSpawnerSetup.ActorsToSpawn.Default;
		}

		int32 ActorSetupIndex = 0;
		TArray<int32> SpawnedActorsNumberList = {};
		SpawnedActorsNumberList.Init(0, MovingActorsSpawnerSetupList.Num());

		for (int32 ActorIndex = 0; ActorIndex < ActorsToSpawnNumber; ++ActorIndex)
		{
			// Select actor to spawn
			FNebulaToolMovingActorsSpawnerSetup MovingActorsSpawnerSetup = {};
			if (bRandomSpawn)
			{
				do
				{
					ActorSetupIndex = UKismetMathLibrary::RandomIntegerInRange(0, MovingActorsSpawnerSetupList.Num() - 1);
				} while (SpawnedActorsNumberList[ActorSetupIndex] >= MovingActorsSpawnerSetupList[ActorSetupIndex].ActorsToSpawn.Default);

				MovingActorsSpawnerSetup = MovingActorsSpawnerSetupList[ActorSetupIndex];
			}
			else
			{
				if (SpawnedActorsNumberList[ActorSetupIndex] >= MovingActorsSpawnerSetupList[ActorSetupIndex].ActorsToSpawn.Default)
				{
					++ActorSetupIndex;
				}

				MovingActorsSpawnerSetup = MovingActorsSpawnerSetupList[ActorSetupIndex];
			}

			// Spawn actor
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.OverrideLevel = GetLevel();

			if(!GetWorld()) return;

			AActor* ActorToSpawn = GetWorld()->SpawnActor<AActor>(MovingActorsSpawnerSetup.ActorToSpawnClass, ActorSpawnParameters);
			if (ActorToSpawn)
			{
				SpawnedActorList.Add(ActorToSpawn);

				SpawnedActorsNumberList[ActorSetupIndex] = SpawnedActorsNumberList[ActorSetupIndex] + 1;

				ActorToSpawn->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

				const float SplineLength = SplineComponent->GetSplineLength();

				// Actor distance from spline start
				float ActorDistanceAlongSpline = 0.0f;
				if (ActorsDistance > 0.0f && ActorsDistance * ActorsToSpawnNumber < SplineLength)
				{
					ActorDistanceAlongSpline = ActorIndex * ActorsDistance;
				}
				else
				{
					ActorDistanceAlongSpline = SplineLength * ActorIndex / ActorsToSpawnNumber;
				}

				ActorDistanceAlongSplineList.Add(ActorDistanceAlongSpline);

				// Actor transform on spline
				const FVector ActorLocation = SplineComponent->GetLocationAtDistanceAlongSpline(ActorDistanceAlongSpline, ESplineCoordinateSpace::Local) + MovingActorsSpawnerSetup.ActorsTransform.GetLocation();

				const FRotator ActorRotation = UKismetMathLibrary::ComposeRotators(SplineComponent->GetRotationAtDistanceAlongSpline(ActorDistanceAlongSpline, ESplineCoordinateSpace::Local), MovingActorsSpawnerSetup.ActorsTransform.Rotator());

				const FVector ActorScale = MovingActorsSpawnerSetup.ActorsTransform.GetScale3D();

				ActorToSpawn->SetActorRelativeLocation(ActorLocation);
				ActorToSpawn->SetActorRelativeRotation(ActorRotation);
				ActorToSpawn->SetActorRelativeScale3D(ActorScale);
			}
			else
			{
				UE_LOG(LogNebulaTool, Warning, TEXT("Actor not spawned."));
			}
		}
	}
}

void ANebulaToolMovingActorsSpawner::DestroySpawnedChildren()
{
	TArray<AActor*>	SpawnedActorList = {};
	GetSpawnedActorList(SpawnedActorList);

	for (AActor* SpawnedActor : SpawnedActorList)
	{
		if (SpawnedActor)
		{
			SpawnedActor->Destroy();
			SpawnedActor = nullptr;
		}
	}

	ActorDistanceAlongSplineList.Empty();

	Super::DestroySpawnedChildren();
}

#undef LOCTEXT_NAMESPACE