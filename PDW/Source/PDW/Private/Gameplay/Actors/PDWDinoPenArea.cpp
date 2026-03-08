// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWDinoPenArea.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"

APDWDinoPenArea::APDWDinoPenArea()
{

}

void APDWDinoPenArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	OnPenAreaBeginOverlap.Broadcast(OtherActor);
}

void APDWDinoPenArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	OnPenAreaEndOverlap.Broadcast(OtherActor);
}

bool APDWDinoPenArea::IsPlayerInsideArea()
{
	TSet<AActor*> CurrentOverlappingActors;
	GetOverlappingActors(CurrentOverlappingActors, APawn::StaticClass());

	if (CurrentOverlappingActors.Num())
	{
		for (AActor* OverlappingActor : CurrentOverlappingActors)
		{
			if (IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(OverlappingActor))
			{
				return true;
			}
		}
	}

	return false;
}

void APDWDinoPenArea::BeginPlay()
{
	Super::BeginPlay();

	FVector Origin = FVector::ZeroVector;
	FVector BoxExtent = FVector::ZeroVector;

	GetActorBounds(false,Origin,BoxExtent);
	PenAreaBounds.SOSearchBox.Min = Origin - BoxExtent;
	PenAreaBounds.SOSearchBox.Max = Origin + BoxExtent;

	PenAreaBounds.PenAreaCenter = Origin;
	PenAreaBounds.MaxMoveRadius = FVector::Dist(PenAreaBounds.SOSearchBox.Max, PenAreaBounds.SOSearchBox.Min) / 2.f;
}
