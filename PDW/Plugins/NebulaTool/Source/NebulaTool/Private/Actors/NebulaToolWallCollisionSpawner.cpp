#include "Actors/NebulaToolWallCollisionSpawner.h"

#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInterface.h"
#include "NebulaTool.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ProceduralMeshComponent.h"

#define LOCTEXT_NAMESPACE "NebulaToolWallCollisionSpawner"

void ANebulaToolWallCollisionSpawner::OnComponentHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{

}

void ANebulaToolWallCollisionSpawner::OnComponentBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ANebulaToolWallCollisionSpawner::OnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{

}

void ANebulaToolWallCollisionSpawner::Spawn()
{
	Super::Spawn();

	if (!GetWorld()->IsGameWorld())
	{
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

		for (int32 SplinePointIndex = 0; SplinePointIndex < SplineComponent->GetNumberOfSplinePoints(); ++SplinePointIndex)
		{
			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
			if (SplineMeshComponent)
			{
				AddInstanceComponent(SplineMeshComponent);

				SplineMeshComponent->SetCanEverAffectNavigation(bCanEverAffectNavigation);
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				SplineMeshComponent->SetHiddenInGame(true);
				SplineMeshComponent->SetCastShadow(false);
				SplineMeshComponent->SetCollisionProfileName(CollisionProfileName);
				SplineMeshComponent->SetCollisionObjectType(CollisionChannel);
				SplineMeshComponent->bApplyImpulseOnDamage = false;
				SplineMeshComponent->bReturnMaterialOnMove = bReturnMaterialOnMove;
				SplineMeshComponent->SetupAttachment(RootComponent);
				SplineMeshComponent->RegisterComponent();

				if (Material)
				{
					SplineMeshComponent->SetMaterial(0, Material);
				}

				if (PhysicalMaterial)
				{
					SplineMeshComponent->SetPhysMaterialOverride(PhysicalMaterial);
				}

				if (bBroadcastCollisionEvents)
				{
					SplineMeshComponent->OnComponentHit.AddDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentHit);
					SplineMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentBeginOverlap);
					SplineMeshComponent->OnComponentEndOverlap.AddDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentEndOverlap);
				}
				else if (SplineMeshComponent->OnComponentHit.IsBound() && SplineMeshComponent->OnComponentBeginOverlap.IsBound() && SplineMeshComponent->OnComponentEndOverlap.IsBound())
				{
					SplineMeshComponent->OnComponentHit.RemoveDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentHit);
					SplineMeshComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentBeginOverlap);
					SplineMeshComponent->OnComponentEndOverlap.RemoveDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentEndOverlap);
				}

				UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"), nullptr, LOAD_None, nullptr);
				if (CubeMesh)
				{
					SplineMeshComponent->SetStaticMesh(CubeMesh);

					// Cube has the pivot in the center
					SplineMeshComponent->SetRelativeLocation(FVector::UpVector * WallSize.Y / 2.0f);

					// Set mesh transform and edit shape in order to follow that of spline
					const FVector StartLocation = SplineComponent->GetLocationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::Local);
					const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
					const FVector EndLocation = SplineComponent->GetLocationAtSplineInputKey(SplinePointIndex + 1, ESplineCoordinateSpace::Local);
					const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplinePointIndex + 1, ESplineCoordinateSpace::Local);
					SplineMeshComponent->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);

					const float StartRoll = UKismetMathLibrary::DegreesToRadians(SplineComponent->GetRotationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local).Roll);
					const float EndRoll = UKismetMathLibrary::DegreesToRadians(SplineComponent->GetRotationAtSplinePoint(SplinePointIndex + 1, ESplineCoordinateSpace::Local).Roll);
					SplineMeshComponent->SetStartRoll(StartRoll);
					SplineMeshComponent->SetEndRoll(EndRoll);

					// Cube has a size of 100cm
					const FVector2D Scale = FVector2D(WallSize.X, WallSize.Y) / 100.0f;
					SplineMeshComponent->SetStartScale(Scale);
					SplineMeshComponent->SetEndScale(Scale);
				}
				else
				{
					UE_LOG(LogNebulaTool, Error, TEXT("Cube mesh not loaded."));
				}
			}
			else
			{
				UE_LOG(LogNebulaTool, Warning, TEXT("SplineMeshComponent not spawned."));
			}
		}
	}
}

void ANebulaToolWallCollisionSpawner::DestroySpawnedChildren()
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
				StaticMeshComponent->OnComponentHit.RemoveDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentHit);
			}
			if (StaticMeshComponent->OnComponentBeginOverlap.IsBound())
			{
				StaticMeshComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentBeginOverlap);
			}
			if (StaticMeshComponent->OnComponentEndOverlap.IsBound())
			{
				StaticMeshComponent->OnComponentEndOverlap.RemoveDynamic(this, &ANebulaToolWallCollisionSpawner::OnComponentEndOverlap);
			}

			StaticMeshComponent->DestroyComponent();
			StaticMeshComponent = nullptr;
		}
	}

	Super::DestroySpawnedChildren();
}

#undef LOCTEXT_NAMESPACE