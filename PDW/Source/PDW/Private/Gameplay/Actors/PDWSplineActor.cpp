// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWSplineActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gameplay/Actors/PDWSplineObstacles.h"
#include "Gameplay/Components/PDWPreyComponent.h"

// Sets default values
APDWSplineActor::APDWSplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MainSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("MainSplineComponent"));
	SetRootComponent(MainSplineComponent);
	PreySplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("PreySplineComponent"));
	PreySplineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APDWSplineActor::BeginPlay()
{
	Super::BeginPlay();
	FakeChase = FindComponentByTag<USkeletalMeshComponent>(TEXT("FakeChase"));
}


void APDWSplineActor::AdjustSpline(USplineComponent* Spline, float DeltaDistance)
{
	Spline->SetWorldTransform(MainSplineComponent->GetComponentTransform());
	Spline->ClearSplinePoints(true);
	int32 AddedPointIndex = 0;
	float Step = 1.f / (SplineConfig.IntermediatePoints + 1);
	float MaxInputKey = static_cast<float>(MainSplineComponent->GetNumberOfSplinePoints() - 1);
	MaxInputKey += MainSplineComponent->IsClosedLoop() ? 1.f : 0.f;
	int32 NumPointsToAdd = FMath::RoundToInt(MaxInputKey / Step);
	NumPointsToAdd += MainSplineComponent->IsClosedLoop() ? 0.0f : 1.0f;
	for (int32 i = 0.f; i < NumPointsToAdd; ++i)
	{
		//FVector RightVector = MainSpline->GetRightVectorAtSplineInputKey(i * Step, ESplineCoordinateSpace::Local);
		//RightVector.Normalize();
		
		FVector MainScale = MainSplineComponent->GetScaleAtSplineInputKey(i * Step);
		FVector MainTangent = MainSplineComponent->GetTangentAtSplineInputKey(i * Step, ESplineCoordinateSpace::Local) * Step;
		FVector RightVector = FVector::CrossProduct(MainSplineComponent->GetUpVectorAtDistanceAlongSpline(i * Step, ESplineCoordinateSpace::Local), MainTangent);
		RightVector.Normalize();
		const FTransform& Transform = MainSplineComponent->GetTransformAtSplineInputKey(i * Step, ESplineCoordinateSpace::Local);
		const FVector& NewPoint = Transform.GetLocation() + RightVector * DeltaDistance * MainScale.Y;
		Spline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);
		Spline->SetRotationAtSplinePoint(i, Transform.GetRotation().Rotator(), ESplineCoordinateSpace::Local, false);
		Spline->SetTangentAtSplinePoint(i, MainSplineComponent->GetTangentAtSplineInputKey(i * Step, ESplineCoordinateSpace::Local) * Step, ESplineCoordinateSpace::Local, false);
		Spline->SetScaleAtSplinePoint(i, MainSplineComponent->GetScaleAtSplinePoint(i * Step), false);
		Spline->SetClosedLoop(MainSplineComponent->IsClosedLoop(), false);
	}
	Spline->UpdateSpline();
}

// Called every frame
void APDWSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APDWSplineActor::GetSplineConfiguration(FPDWSplineConfiguration& outConfig)
{
	outConfig = SplineConfig;
}

void APDWSplineActor::CreateSplines()
{
	int32 Index = 0;
	int32 Sign = Index - 1; 
	MainSplineComponent->ComponentTags.Empty();
	for (const auto& SplineConfiguration : SplineConfig.SplineTagsAndColor)
	{
		if (Index == 1)
		{
#if WITH_EDITOR
			MainSplineComponent->EditorUnselectedSplineSegmentColor = SplineConfiguration.Value;
#endif
			MainSplineComponent->ComponentTags.Add(SplineConfiguration.Key.GetTagName());
			SplineComponents.Add(Index,MainSplineComponent);

		}
		else
		{
			USplineComponent* SplineZero = NewObject<USplineComponent>(this);
			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget,true);
			SplineZero->RegisterComponent();
			SplineZero->AttachToComponent(MainSplineComponent,AttachRules);
			AdjustSpline(SplineZero,Sign*SplineConfig.DistanceBetweenSplines);
			/*SplineZero->SplineCurves = MainSplineComponent->SplineCurves;
			SplineZero->SetRelativeLocation(FVector(0,Sign*SplineConfig.DistanceBetweenSplines,0));*/
#if WITH_EDITOR
			SplineZero->EditorUnselectedSplineSegmentColor = SplineConfiguration.Value;
#endif
			SplineZero->ComponentTags.Add(SplineConfiguration.Key.GetTagName());
			AddInstanceComponent(SplineZero);
			SplineComponents.Add(Index,SplineZero);
		}
		++Index;
		++Sign;
	}
	MarkPackageDirty();
}

void APDWSplineActor::SpawnActorOnSpline()
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.OverrideLevel = GetLevel();
	for (int32 i = 0; i < ObstaclesConfig.Num(); ++i)
	{
		FPDWActorsList Actors;
		SpawnedObstacles.Add(i,Actors);
		USplineComponent* SplineToUse = FindComponentByTag<USplineComponent>(ObstaclesConfig[i].SplineTag.GetTagName());
		float SplineLength = ObstaclesConfig[i].PositionRules == ESplinePositionRule::UseDistance ? SplineToUse->GetSplineLength() : 0.0f;
		for (int32 ActorIndex = 0; ActorIndex < ObstaclesConfig[i].ActorsToSpawn.Num(); ++ActorIndex)
		{

			float CurrentDistance = ObstaclesConfig[i].MinStartingDistanceFromOrigin;
			AActor* ActorToSpawn = nullptr;
			while (CurrentDistance < SplineLength)
			{

				if (ObstaclesConfig[i].bRandomizePosition)
				{
					const float Value = FMath::RandRange(0.0, 1.0);
					CurrentDistance += FMath::RandRange(ObstaclesConfig[i].DistanceRange.GetLowerBoundValue(), ObstaclesConfig[i].DistanceRange.GetUpperBoundValue());
					if (Value <= ObstaclesConfig[i].Probability)
					{
						ActorToSpawn = GetWorld()->SpawnActor<AActor>(ObstaclesConfig[i].ActorsToSpawn[ActorIndex], ActorSpawnParameters);
					}
				}
				else
				{
					ActorToSpawn = GetWorld()->SpawnActor<AActor>(ObstaclesConfig[i].ActorsToSpawn[ActorIndex], ActorSpawnParameters);
				}


				if (APDWSplineObstacles* SplineObstacle = Cast<APDWSplineObstacles>(ActorToSpawn))
				{
					SplineObstacle->SetSplineOwner(SplineToUse);
				}
				if (ActorToSpawn)
				{
					ActorToSpawn->AttachToComponent(SplineToUse, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

					const FTransform& ActroTransform = GetTransformOnSpline(SplineToUse, ObstaclesConfig[i].bSkipOriginPlacement, ObstaclesConfig[i].PositionRules, ESplineCoordinateSpace::Local, ActorIndex, ObstaclesConfig[i].ActorsToSpawn.Num(), SplineLength, CurrentDistance);
					ActorToSpawn->SetActorRelativeLocation(ActroTransform.GetLocation());
					ActorToSpawn->SetActorRelativeRotation(ActroTransform.GetRotation());
					ActorToSpawn->SetActorScale3D(FVector::OneVector);
					SpawnedObstacles[i].Obstacles.AddUnique(ActorToSpawn);
				}
			}
		}
	}
	MarkPackageDirty();
}

void APDWSplineActor::UpdateSplines()
{
	int32 Index = 0;
	for (const auto& [SplineIndex,CachedSplineComponent] : SplineComponents)
	{
		//if (SplineIndex == 1)
		//{
		//	continue;
		//}
		CachedSplineComponent->SplineCurves = MainSplineComponent->SplineCurves;
		UpdateActorsPositions(SplineIndex, CachedSplineComponent);
		++Index;
	}
	MarkPackageDirty();
}

void APDWSplineActor::ClearSpawnedActors()
{
	for(auto& [SplineIndex, ObstaclesList] : SpawnedObstacles)
	{
		for (AActor* Obstacle : ObstaclesList.Obstacles)
		{
			if (Obstacle)
			{
				Obstacle->Destroy();
			}
		}
	}
}

UPDWPreyComponent* APDWSplineActor::SpawnPrey(TSubclassOf<AActor> inPreyToSpawn)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Actor = GetWorld()->SpawnActor<AActor>(inPreyToSpawn, PreySplineComponent->GetComponentLocation(), FRotator::ZeroRotator, SpawnParams);
	UPDWPreyComponent* Comp = Actor->FindComponentByClass<UPDWPreyComponent>();
	if (Comp)
	{
		Comp->SetSpline(PreySplineComponent);
	}
	return  Comp;
}

TArray<AActor*> APDWSplineActor::GetPlayersEndPoint() const
{
	return PlayersEndPoint;
}

void APDWSplineActor::ResetObstacles()
{
	for(auto& [SplineIndex, ObstaclesList] : SpawnedObstacles)
	{
		for (AActor* Obstacle : ObstaclesList.Obstacles)
		{
			if (Obstacle)
			{
				Obstacle->SetActorHiddenInGame(false);
			}
		}
	}
}

void APDWSplineActor::UpdateActorsPositions(int32 SplineIndex,USplineComponent* inSplineComponent)
{
	if(!SpawnedObstacles.Contains(SplineIndex))
	{
		return;
	}

	for (AActor* Obstacle : SpawnedObstacles[SplineIndex].Obstacles)
	{
		if (Obstacle)
		{
			const FVector NewLocation = inSplineComponent->FindLocationClosestToWorldLocation(Obstacle->GetActorLocation(), ESplineCoordinateSpace::Local);
			const FRotator NewRotation = UKismetMathLibrary::ComposeRotators(inSplineComponent->FindRotationClosestToWorldLocation(Obstacle->GetActorLocation(), ESplineCoordinateSpace::Local), FRotator::ZeroRotator);
			Obstacle->SetActorRelativeLocation(NewLocation);
			Obstacle->SetActorRelativeRotation(NewRotation);
		}
	}
}

FTransform APDWSplineActor::GetTransformOnSpline(USplineComponent* inSpline,bool bSkipFirstPlacement,const ESplinePositionRule& inPositionRule,ESplineCoordinateSpace::Type CoordinateSpace,int32 ActorIndex,int32 MaxActorToSpawn /*= 0*/,float SplineLenght/* = 0.0f*/, float Distance /*= 0.0f*/)
{
	FTransform Transform;
	if (bSkipFirstPlacement)
	{
		ActorIndex++;
	}
	if (inPositionRule == ESplinePositionRule::UseDistance)
	{
		float ActorDistanceAlongSpline = 0.0f;
		if (Distance > 0.0f && Distance * MaxActorToSpawn < SplineLenght)
		{
			ActorDistanceAlongSpline = (ActorIndex) * Distance;
		}
		else
		{
			ActorDistanceAlongSpline = (SplineLenght * (ActorIndex)) / MaxActorToSpawn;
		}

		Transform.SetLocation(inSpline->GetLocationAtDistanceAlongSpline(ActorDistanceAlongSpline, CoordinateSpace));
		Transform.SetRotation(FQuat::MakeFromRotator(UKismetMathLibrary::ComposeRotators(inSpline->GetRotationAtDistanceAlongSpline(ActorDistanceAlongSpline, CoordinateSpace), FRotator::ZeroRotator)));
	}
	else if (inPositionRule == ESplinePositionRule::UseSplinePoint)
	{
		Transform.SetLocation(inSpline->GetLocationAtSplinePoint(ActorIndex, CoordinateSpace));
		Transform.SetRotation(FQuat::MakeFromRotator(UKismetMathLibrary::ComposeRotators(inSpline->GetRotationAtSplinePoint(ActorIndex, CoordinateSpace), FRotator::ZeroRotator)));
	}
	Transform.SetScale3D(FVector::OneVector);

	return Transform;
}
