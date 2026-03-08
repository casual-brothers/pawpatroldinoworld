// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWAudioRiverActor.h"
#include "Components/SplineComponent.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"

// Sets default values
APDWAudioRiverActor::APDWAudioRiverActor()
{
	PrimaryActorTick.bCanEverTick = true;

    // Create spline
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
    SplineComponent->SetClosedLoop(false);

}

// Called when the game starts or when spawned
void APDWAudioRiverActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APDWAudioRiverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	ensure(GameMode);
	
	PlayerIndex++;
	PlayerIndex %= 2;
	APDWPlayerController* Controller = GameMode->GetPlayerControllerTwo();
	bool bMultiplayer = Controller->IsValidLowLevel();
	if (!bMultiplayer || PlayerIndex == 0)
	{
		PlayerIndex = 0;
		Controller = GameMode->GetPlayerControllerOne();
		LocationFromPlayer1 = Controller->GetPawn()->GetActorLocation();
		MinDistanceFromPlayer1 = ComputeDistance(LocationFromPlayer1);
	}
	else
	{
		LocationFromPlayer2 = Controller->GetPawn()->GetActorLocation();
		MinDistanceFromPlayer2 = ComputeDistance(LocationFromPlayer2);
	}
	if (!bMultiplayer || MinDistanceFromPlayer1 < MinDistanceFromPlayer2)
	{
		UpdateAudio(MinDistanceFromPlayer1, LocationFromPlayer1);
	}
	else
	{
		UpdateAudio(MinDistanceFromPlayer2, LocationFromPlayer2);
	}
}

void APDWAudioRiverActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (GIsEditor && !GetWorld()->IsGameWorld())
	{
		ComputedSplinePoints = TArray<FVector>();
		float Distance = 0.f;
		while (Distance < SplineComponent->GetSplineLength())
		{
			ComputedSplinePoints.Add(SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World));
			Distance += SplinePointsDistance;
		}
		MinAudioVolumeDistanceSquared = MinAudioVolumeDistance * MinAudioVolumeDistance;
		MaxAudioVolumeDistanceSquared = MaxAudioVolumeDistance * MaxAudioVolumeDistance;
	}
#endif
}


float APDWAudioRiverActor::ComputeDistance(FVector& PlayerPawnLocation)
{
	float MinDistanceSquared = MinAudioVolumeDistanceSquared;
	for(FVector& Point : ComputedSplinePoints)
	{
		float CurrentDistanceSquared = FVector::DistSquared(Point, PlayerPawnLocation);
		if (CurrentDistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = CurrentDistanceSquared;
			
			if (MinDistanceSquared < MaxAudioVolumeDistanceSquared)
			{
				return MaxAudioVolumeDistance;
			}
		}
	}
	return FMath::Sqrt(MinDistanceSquared);
}

void APDWAudioRiverActor::UpdateAudio(float Distance, FVector& Location)
{
	float NewVolume = FMath::Clamp(
		(MinAudioVolumeDistance - Distance) /
		(MinAudioVolumeDistance - MaxAudioVolumeDistance),
		0.f, 1.f);
	if (NewVolume <= 0.01f && AudioInstance.Instance->isValid())
	{
		AudioInstance.Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
	if (NewVolume >= 0.01f && CurrentVolume<=0.01f)
	{
		AudioInstance = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), AudioEvent, true);
		AudioInstance.Instance->setParameterByName("RiverVolume", NewVolume);
	}
	else if(AudioInstance.Instance->isValid())
	{
		AudioInstance.Instance->setParameterByName("RiverVolume", NewVolume);
	}
}


void APDWAudioRiverActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AudioInstance.Instance->isValid())
	{
		AudioInstance.Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
	Super::EndPlay(EndPlayReason);
}

void APDWAudioRiverActor::Debug()
{	
#if WITH_EDITOR
	if (bDebugMin)
	{
		for(int32 i=1; i < ComputedSplinePoints.Num(); i++)
		{
			DrawDebugLine(GetWorld(), ComputedSplinePoints[i-1], ComputedSplinePoints[i], FColor::Green, false, 10.f, (uint8)0U, MinAudioVolumeDistance);
		}
	}
	else if (bDebugMax)
	{
		for(int32 i=1; i < ComputedSplinePoints.Num(); i++)
		{
			DrawDebugLine(GetWorld(), ComputedSplinePoints[i-1], ComputedSplinePoints[i], FColor::Yellow, false, 10.f, (uint8)0U, MaxAudioVolumeDistance);
		}
	}
#endif
}
