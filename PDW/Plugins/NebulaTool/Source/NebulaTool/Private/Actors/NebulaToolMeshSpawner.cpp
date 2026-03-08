#include "Actors/NebulaToolMeshSpawner.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NebulaTool.h"
#include "UObject/NoExportTypes.h"

#define LOCTEXT_NAMESPACE "NebulaToolMeshSpawner"

void ANebulaToolMeshSpawner::OnComponentHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{

}

void ANebulaToolMeshSpawner::OnComponentBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ANebulaToolMeshSpawner::OnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
}

void ANebulaToolMeshSpawner::Spawn()
{
	Super::Spawn();

	for (FNebulaToolMeshSpawnerSetup& MeshSpawnerSetup : MeshSpawnerSetupList)
	{
		// Spline points number
		int32 SplinePointsNumber = 0;
		if (MeshSpawnerSetup.SpawnType == ENebulaToolMeshSpawnerSpawnType::SplinePoints)
		{
			SplinePointsNumber = SplineComponent->GetNumberOfSplinePoints();
		}
		else if (MeshSpawnerSetup.SpawnType == ENebulaToolMeshSpawnerSpawnType::SplineDistance)
		{
			SplinePointsNumber = UKismetMathLibrary::FTrunc(SplineComponent->GetSplineLength() / MeshSpawnerSetup.MeshDistance) + 1;
		}
		else if (MeshSpawnerSetup.SpawnType == ENebulaToolMeshSpawnerSpawnType::FixedNumber)
		{
			SplinePointsNumber = MeshSpawnerSetup.bFollowSplineShape ? MeshSpawnerSetup.MeshToSpawn + 1 : MeshSpawnerSetup.MeshToSpawn;
		}

		MeshSpawnerSetup.SplinePointList.Empty();

		for (int32 SplinePointIndex = 0; SplinePointIndex < SplinePointsNumber; ++SplinePointIndex)
		{
			// Mesh transform on spline
			FVector MeshLocation = FVector::ZeroVector;
			FRotator MeshRotator = FRotator::ZeroRotator;
			FVector MeshScale = FVector::ZeroVector;

			if (MeshSpawnerSetup.SpawnType == ENebulaToolMeshSpawnerSpawnType::SplinePoints)
			{
				MeshLocation = SplineComponent->GetLocationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
				MeshRotator = SplineComponent->GetRotationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
			}
			else if (MeshSpawnerSetup.SpawnType == ENebulaToolMeshSpawnerSpawnType::SplineDistance)
			{
				const float Distance = SplinePointIndex * MeshSpawnerSetup.MeshDistance;
				MeshLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
				MeshRotator = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
			}
			else if (MeshSpawnerSetup.SpawnType == ENebulaToolMeshSpawnerSpawnType::FixedNumber)
			{
				const float Time = SplinePointIndex * (SplineComponent->Duration / MeshSpawnerSetup.MeshToSpawn);
				MeshLocation = SplineComponent->GetLocationAtTime(Time, ESplineCoordinateSpace::Local, true);
				MeshRotator = SplineComponent->GetRotationAtTime(Time, ESplineCoordinateSpace::Local, true);
			}

			FRandomStream RandomStream = {};
			if (MeshSpawnerSetup.bUseRandomStream)
			{
				RandomStream = UKismetMathLibrary::MakeRandomStream(MeshSpawnerSetup.RandomStream + SplinePointIndex);

				const FVector RandomLocation = FVector(
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.GetLocation().X, -MeshSpawnerSetup.TransformRandom.GetLocation().X),
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.GetLocation().Y, -MeshSpawnerSetup.TransformRandom.GetLocation().Y),
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.GetLocation().Z, -MeshSpawnerSetup.TransformRandom.GetLocation().Z));

				MeshLocation += MeshRotator.RotateVector(RandomLocation + MeshSpawnerSetup.Transform.GetLocation());

				const FRotator RandomRotation = FRotator(
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.Rotator().Roll, -MeshSpawnerSetup.TransformRandom.Rotator().Roll),
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.Rotator().Pitch, -MeshSpawnerSetup.TransformRandom.Rotator().Pitch),
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.Rotator().Yaw, -MeshSpawnerSetup.TransformRandom.Rotator().Yaw));

				MeshRotator = UKismetMathLibrary::NormalizedDeltaRotator(MeshRotator, UKismetMathLibrary::ComposeRotators(MeshSpawnerSetup.Transform.Rotator(), RandomRotation));

				const FVector RandomScale = FVector(
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.GetScale3D().X, -MeshSpawnerSetup.TransformRandom.GetScale3D().X),
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.GetScale3D().Y, -MeshSpawnerSetup.TransformRandom.GetScale3D().Y),
					UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, MeshSpawnerSetup.TransformRandom.GetScale3D().Z, -MeshSpawnerSetup.TransformRandom.GetScale3D().Z));

				MeshScale = RandomScale + MeshSpawnerSetup.Transform.GetScale3D();
			}
			else
			{
				MeshLocation += MeshRotator.RotateVector(MeshSpawnerSetup.Transform.GetLocation());

				MeshRotator = UKismetMathLibrary::NormalizedDeltaRotator(MeshRotator, MeshSpawnerSetup.Transform.Rotator());

				MeshScale = MeshSpawnerSetup.Transform.GetScale3D();
			}

			if (MeshSpawnerSetup.bSnapToGround)
			{
				const FVector& StartPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), MeshLocation);
				const FVector& EndPoint = StartPoint + FVector(0.0f, 0.0f, -10000.0f);

				FHitResult HitResult = {};

				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(this);

				GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, MeshSpawnerSetup.GroundCollisionChannel, CollisionQueryParams);
				if (HitResult.bBlockingHit)
				{
					MeshLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), HitResult.ImpactPoint);

					FVector XVector = UKismetMathLibrary::GetForwardVector(MeshRotator);
					FVector ZVector = UKismetMathLibrary::GetUpVector(UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal)));
					MeshRotator = UKismetMathLibrary::MakeRotFromXZ(XVector, ZVector);
				}
			}

			FSplinePoint SplinePoint = FSplinePoint(SplinePointIndex, MeshLocation, ESplinePointType::Curve, MeshRotator, MeshScale);
			MeshSpawnerSetup.SplinePointList.Add(SplinePoint);
		}

		if (MeshSpawnerSetup.bFollowSplineShape)
		{
			MeshSpawnerSetup.InternalSplineComponent = NewObject<USplineComponent>(this);
			if (MeshSpawnerSetup.InternalSplineComponent)
			{
				MeshSpawnerSetup.InternalSplineComponent->SetMobility(EComponentMobility::Movable);
				MeshSpawnerSetup.InternalSplineComponent->SetRelativeScale3D(FVector::ZeroVector);
				MeshSpawnerSetup.InternalSplineComponent->SetupAttachment(RootComponent);
				MeshSpawnerSetup.InternalSplineComponent->RegisterComponent();

				// Add points to InternalSplineComponent
				MeshSpawnerSetup.InternalSplineComponent->ClearSplinePoints(false);
				MeshSpawnerSetup.InternalSplineComponent->AddPoints(MeshSpawnerSetup.SplinePointList);

				for (int32 SplinePointIndex = 0; SplinePointIndex < SplinePointsNumber - 1; ++SplinePointIndex)
				{
					// Create mesh component
					USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
					if (SplineMeshComponent)
					{
						AddInstanceComponent(SplineMeshComponent);

						SplineMeshComponent->SetCanEverAffectNavigation(MeshSpawnerSetup.bCanEverAffectNavigation);
						SplineMeshComponent->SetMobility(EComponentMobility::Movable);
						SplineMeshComponent->SetupAttachment(RootComponent);
						SplineMeshComponent->RegisterComponent();

						// Set mesh and materials
						if (SplinePointIndex == 0 && MeshSpawnerSetup.StartMesh != nullptr)
						{
							SplineMeshComponent->SetStaticMesh(MeshSpawnerSetup.StartMesh);
							for (auto& StartMeshMaterial : MeshSpawnerSetup.StartMeshMaterialMap)
							{
								SplineMeshComponent->SetMaterialByName(StartMeshMaterial.Key, StartMeshMaterial.Value);
							}
						}
						else if (SplinePointIndex == SplinePointsNumber - 2 && MeshSpawnerSetup.EndMesh != nullptr)
						{
							SplineMeshComponent->SetStaticMesh(MeshSpawnerSetup.EndMesh);
							for (auto& EndMeshMaterial : MeshSpawnerSetup.EndMeshMaterialMap)
							{
								SplineMeshComponent->SetMaterialByName(EndMeshMaterial.Key, EndMeshMaterial.Value);
							}
						}
						else if (MeshSpawnerSetup.MainMesh != nullptr)
						{
							SplineMeshComponent->SetStaticMesh(MeshSpawnerSetup.MainMesh);
							for (auto& MainMeshMaterial : MeshSpawnerSetup.MainMeshMaterialMap)
							{
								SplineMeshComponent->SetMaterialByName(MainMeshMaterial.Key, MainMeshMaterial.Value);
							}
						}

						// Set mesh transform and edit shape in order to follow that of spline
						const FVector StartLocation = MeshSpawnerSetup.InternalSplineComponent->GetLocationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
						const FVector StartTangent = MeshSpawnerSetup.InternalSplineComponent->GetTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
						const FVector EndLocation = MeshSpawnerSetup.InternalSplineComponent->GetLocationAtSplinePoint(SplinePointIndex + 1, ESplineCoordinateSpace::Local);
						const FVector EndTangent = MeshSpawnerSetup.InternalSplineComponent->GetTangentAtSplinePoint(SplinePointIndex + 1, ESplineCoordinateSpace::Local);
						SplineMeshComponent->SetStartAndEnd(StartLocation, StartTangent, EndLocation + MeshSpawnerSetup.MeshLenghtAdjustment, EndTangent);

						const float StartRoll = UKismetMathLibrary::DegreesToRadians(MeshSpawnerSetup.InternalSplineComponent->GetRotationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local).Roll);
						const float EndRoll = UKismetMathLibrary::DegreesToRadians(MeshSpawnerSetup.InternalSplineComponent->GetRotationAtSplinePoint(SplinePointIndex + 1, ESplineCoordinateSpace::Local).Roll);
						SplineMeshComponent->SetStartRoll(StartRoll);
						SplineMeshComponent->SetEndRoll(EndRoll);

						const FVector2D Scale = FVector2D(MeshSpawnerSetup.InternalSplineComponent->GetScaleAtSplinePoint(SplinePointIndex).X, MeshSpawnerSetup.InternalSplineComponent->GetScaleAtSplinePoint(SplinePointIndex).Y);
						SplineMeshComponent->SetStartScale(Scale);
						SplineMeshComponent->SetEndScale(Scale);

						// Rendering setup
						ApplyRenderingSetup(SplineMeshComponent, MeshSpawnerSetup);

						// Collision setup
						ApplyCollisionSetup(SplineMeshComponent, MeshSpawnerSetup);
					}
					else
					{
						UE_LOG(LogNebulaTool, Warning, TEXT("SplineMeshComponent not spawned."));
					}
				}
			}
			else
			{
				UE_LOG(LogNebulaTool, Warning, TEXT("InternalSplineComponent not spawned."));
			}
		}
		else
		{
			int32 StartSplinePointIndex = 0;
			int32 EndSplinePointIndex = SplinePointsNumber;
			if (EndSplinePointIndex > StartSplinePointIndex)
			{
				if (MeshSpawnerSetup.StartMesh != nullptr)
				{
					UHierarchicalInstancedStaticMeshComponent* HISMComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
					if (HISMComponent)
					{
						HISMComponent->SetCanEverAffectNavigation(MeshSpawnerSetup.bCanEverAffectNavigation);
						HISMComponent->SetMobility(EComponentMobility::Movable);
						HISMComponent->SetupAttachment(RootComponent);
						HISMComponent->RegisterComponent();

						AddInstanceComponent(HISMComponent);

						// Set mesh and materials
						HISMComponent->SetStaticMesh(MeshSpawnerSetup.StartMesh);
						for (auto& StartMeshMaterial : MeshSpawnerSetup.StartMeshMaterialMap)
						{
							HISMComponent->SetMaterialByName(StartMeshMaterial.Key, StartMeshMaterial.Value);
						}

						// Add instance
						HISMComponent->AddInstance(FTransform(MeshSpawnerSetup.SplinePointList[StartSplinePointIndex].Rotation, MeshSpawnerSetup.SplinePointList[StartSplinePointIndex].Position, MeshSpawnerSetup.SplinePointList[StartSplinePointIndex].Scale));

						// Reverse culling if only one of scale axis is negative
						HISMComponent->SetReverseCulling(((MeshSpawnerSetup.SplinePointList[StartSplinePointIndex].Scale.X < 0.0f) ^ (MeshSpawnerSetup.SplinePointList[StartSplinePointIndex].Scale.Y < 0.0f)) ^ (MeshSpawnerSetup.SplinePointList[StartSplinePointIndex].Scale.Z < 0.0f));

						// Rendering setup
						ApplyRenderingSetup(HISMComponent, MeshSpawnerSetup);

						// Collision setup
						ApplyCollisionSetup(HISMComponent, MeshSpawnerSetup);

						++StartSplinePointIndex;
					}
					else
					{
						UE_LOG(LogNebulaTool, Warning, TEXT("Start HISM not spawned."));
					}
				}

				if (MeshSpawnerSetup.EndMesh != nullptr)
				{
					UHierarchicalInstancedStaticMeshComponent* HISMComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
					if (HISMComponent)
					{
						HISMComponent->SetCanEverAffectNavigation(MeshSpawnerSetup.bCanEverAffectNavigation);
						HISMComponent->SetMobility(EComponentMobility::Movable);
						HISMComponent->SetupAttachment(RootComponent);
						HISMComponent->RegisterComponent();

						AddInstanceComponent(HISMComponent);

						// Set mesh and materials
						HISMComponent->SetStaticMesh(MeshSpawnerSetup.EndMesh);
						for (auto& EndMeshMaterial : MeshSpawnerSetup.EndMeshMaterialMap)
						{
							HISMComponent->SetMaterialByName(EndMeshMaterial.Key, EndMeshMaterial.Value);
						}

						--EndSplinePointIndex;

						// Add instance
						HISMComponent->AddInstance(FTransform(MeshSpawnerSetup.SplinePointList[EndSplinePointIndex].Rotation, MeshSpawnerSetup.SplinePointList[EndSplinePointIndex].Position, MeshSpawnerSetup.SplinePointList[EndSplinePointIndex].Scale));

						// Reverse culling if only one of scale axis is negative
						HISMComponent->SetReverseCulling(((MeshSpawnerSetup.SplinePointList[EndSplinePointIndex].Scale.X < 0.0f) ^ (MeshSpawnerSetup.SplinePointList[EndSplinePointIndex].Scale.Y < 0.0f)) ^ (MeshSpawnerSetup.SplinePointList[EndSplinePointIndex].Scale.Z < 0.0f));

						// Rendering setup
						ApplyRenderingSetup(HISMComponent, MeshSpawnerSetup);

						// Collision setup
						ApplyCollisionSetup(HISMComponent, MeshSpawnerSetup);
					}
					else
					{
						UE_LOG(LogNebulaTool, Warning, TEXT("End HISM not spawned."));
					}
				}

				if (MeshSpawnerSetup.MainMesh != nullptr)
				{
					UHierarchicalInstancedStaticMeshComponent* HISMComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
					if (HISMComponent)
					{
						HISMComponent->SetCanEverAffectNavigation(MeshSpawnerSetup.bCanEverAffectNavigation);
						HISMComponent->SetMobility(EComponentMobility::Movable);
						HISMComponent->SetupAttachment(RootComponent);
						HISMComponent->RegisterComponent();

						AddInstanceComponent(HISMComponent);

						// Set mesh and materials
						HISMComponent->SetStaticMesh(MeshSpawnerSetup.MainMesh);
						for (auto& MainMeshMaterial : MeshSpawnerSetup.MainMeshMaterialMap)
						{
							HISMComponent->SetMaterialByName(MainMeshMaterial.Key, MainMeshMaterial.Value);
						}

						for (int32 SplinePointIndex = StartSplinePointIndex; SplinePointIndex < EndSplinePointIndex; ++SplinePointIndex)
						{
							// Add instance
							HISMComponent->AddInstance(FTransform(MeshSpawnerSetup.SplinePointList[SplinePointIndex].Rotation, MeshSpawnerSetup.SplinePointList[SplinePointIndex].Position, MeshSpawnerSetup.SplinePointList[SplinePointIndex].Scale));

							// Reverse culling if only one of scale axis is negative
							HISMComponent->SetReverseCulling(((MeshSpawnerSetup.SplinePointList[SplinePointIndex].Scale.X < 0.0f) ^ (MeshSpawnerSetup.SplinePointList[SplinePointIndex].Scale.Y < 0.0f)) ^ (MeshSpawnerSetup.SplinePointList[SplinePointIndex].Scale.Z < 0.0f));
						}

						// Rendering setup
						ApplyRenderingSetup(HISMComponent, MeshSpawnerSetup);

						// Collision setup
						ApplyCollisionSetup(HISMComponent, MeshSpawnerSetup);
					}
					else
					{
						UE_LOG(LogNebulaTool, Warning, TEXT("Main HISM not spawned."));
					}
				}
			}
		}
	}
}

void ANebulaToolMeshSpawner::DestroySpawnedChildren()
{
	TArray<UActorComponent*> ActorComponentList = {};
	GetComponents(ActorComponentList);
	for (UActorComponent* ActorComponent : ActorComponentList)
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent);
		if (StaticMeshComponent)
		{
			if (StaticMeshComponent->OnComponentHit.IsBound())
			{
				StaticMeshComponent->OnComponentHit.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentHit);
			}
			if (StaticMeshComponent->OnComponentBeginOverlap.IsBound())
			{
				StaticMeshComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentBeginOverlap);
			}
			if (StaticMeshComponent->OnComponentEndOverlap.IsBound())
			{
				StaticMeshComponent->OnComponentEndOverlap.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentEndOverlap);
			}

			StaticMeshComponent->DestroyComponent();
			StaticMeshComponent = nullptr;
		}
	}

	for (FNebulaToolMeshSpawnerSetup& MeshSpawnerSetup : MeshSpawnerSetupList)
	{
		if (MeshSpawnerSetup.InternalSplineComponent)
		{
			MeshSpawnerSetup.InternalSplineComponent->DestroyComponent();
			MeshSpawnerSetup.InternalSplineComponent = nullptr;
		}
	}

	Super::DestroySpawnedChildren();
}

void ANebulaToolMeshSpawner::ApplyRenderingSetup(UStaticMeshComponent* StaticMeshComponent, const FNebulaToolMeshSpawnerSetup& MeshSpawnerSetup)
{
	StaticMeshComponent->MinDrawDistance = MeshSpawnerSetup.MinDrawDistance.Default;
	StaticMeshComponent->LDMaxDrawDistance = MeshSpawnerSetup.MaxDrawDistance.Default;
	StaticMeshComponent->SetCachedMaxDrawDistance(MeshSpawnerSetup.MaxDrawDistance.Default);
	StaticMeshComponent->SetCastShadow(MeshSpawnerSetup.CastShadow.Default);
	StaticMeshComponent->SetReceivesDecals(MeshSpawnerSetup.bReceiveDecals);
	StaticMeshComponent->SetRenderCustomDepth(MeshSpawnerSetup.bRenderCustomDepthPass);
	StaticMeshComponent->bAffectDistanceFieldLighting = MeshSpawnerSetup.bAffectDistanceFieldLighting;
	StaticMeshComponent->bWorldPositionOffsetWritesVelocity = MeshSpawnerSetup.bWPORendersVelocity;
	StaticMeshComponent->LightingChannels = MeshSpawnerSetup.LightingChannels;
}

void ANebulaToolMeshSpawner::ApplyCollisionSetup(UStaticMeshComponent* StaticMeshComponent, const FNebulaToolMeshSpawnerSetup& MeshSpawnerSetup)
{
	if (MeshSpawnerSetup.bEnableCollision)
	{
		if (MeshSpawnerSetup.PhysicalMaterial)
		{
			StaticMeshComponent->SetPhysMaterialOverride(MeshSpawnerSetup.PhysicalMaterial);
		}

		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		if (MeshSpawnerSetup.CollisionProfileName != NAME_None)
		{
			StaticMeshComponent->SetCollisionProfileName(MeshSpawnerSetup.CollisionProfileName);
		}

		StaticMeshComponent->bReturnMaterialOnMove = MeshSpawnerSetup.bReturnMaterialOnMove;

		if (MeshSpawnerSetup.bBroadcastCollisionEvents)
		{
			StaticMeshComponent->OnComponentHit.AddDynamic(this, &ANebulaToolMeshSpawner::OnComponentHit);
			StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ANebulaToolMeshSpawner::OnComponentBeginOverlap);
			StaticMeshComponent->OnComponentEndOverlap.AddDynamic(this, &ANebulaToolMeshSpawner::OnComponentEndOverlap);
		}
		else if (StaticMeshComponent->OnComponentHit.IsBound() && StaticMeshComponent->OnComponentBeginOverlap.IsBound() && StaticMeshComponent->OnComponentEndOverlap.IsBound())
		{
			StaticMeshComponent->OnComponentHit.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentHit);
			StaticMeshComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentBeginOverlap);
			StaticMeshComponent->OnComponentEndOverlap.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentEndOverlap);
		}
	}
	else
	{
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		StaticMeshComponent->SetCollisionProfileName(NAME_None);

		StaticMeshComponent->bReturnMaterialOnMove = false;

		if (StaticMeshComponent->OnComponentHit.IsBound() && StaticMeshComponent->OnComponentBeginOverlap.IsBound() && StaticMeshComponent->OnComponentEndOverlap.IsBound())
		{
			StaticMeshComponent->OnComponentHit.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentHit);
			StaticMeshComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentBeginOverlap);
			StaticMeshComponent->OnComponentEndOverlap.RemoveDynamic(this, &ANebulaToolMeshSpawner::OnComponentEndOverlap);
		}
	}
}

#undef LOCTEXT_NAMESPACE