#include "Actors/WPTeleportActor.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "WPHelperSubsystem.h"
#include "Kismet/GameplayStatics.h"

AWPTeleportActor::AWPTeleportActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AWPTeleportActor::StartTeleport(TArray<AActor*>& inTeleportingActors, const TArray<FTransform>& inPositions)
{
	EnableStreamingSource();
	TeleportingActors = inTeleportingActors;
	TargetLocations = inPositions;
	SetActorLocationAndRotation(inPositions[0].GetLocation(), inPositions[0].GetRotation());
	WPHelperSubsystem->OnTeleportStarted.Broadcast();
}

void AWPTeleportActor::Teleport()
{
	int32 i = 0;
	for (AActor* TargetActor : TeleportingActors)
	{
		if(i > TargetLocations.Num() - 1)
		{
			i = 0;
		}
		if (!TargetActor)
		{
			continue;
		}
		TargetActor->SetActorLocationAndRotation(TargetLocations[i].GetLocation(), TargetLocations[i].GetRotation());
		i++;
	}
	TeleportingActors.Empty();
	WPHelperSubsystem->OnTeleportCompleted.Broadcast();
}

void AWPTeleportActor::OnStreamingCompleted()
{
	Super::OnStreamingCompleted();
	Teleport();
}

