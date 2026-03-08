// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWSplineObstacles.h"
#include "Kismet/KismetMathLibrary.h"

APDWSplineObstacles::APDWSplineObstacles()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APDWSplineObstacles::SetSplineOwner(USplineComponent* inSplineOwner)
{
	SplineOwner = MakeWeakObjectPtr(const_cast<USplineComponent*>(inSplineOwner));
}

#if WITH_EDITOR
void APDWSplineObstacles::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	if (SplineOwner.Pin())
	{
		const FVector NewLocation = SplineOwner.Get()->FindLocationClosestToWorldLocation(GetActorLocation(),ESplineCoordinateSpace::Local);
		const FRotator NewRotation = UKismetMathLibrary::ComposeRotators(SplineOwner.Get()->FindRotationClosestToWorldLocation(GetActorLocation(),ESplineCoordinateSpace::Local), FRotator::ZeroRotator);
		SetActorRelativeLocation(NewLocation);
		SetActorRelativeRotation(NewRotation);
	}
}
#endif