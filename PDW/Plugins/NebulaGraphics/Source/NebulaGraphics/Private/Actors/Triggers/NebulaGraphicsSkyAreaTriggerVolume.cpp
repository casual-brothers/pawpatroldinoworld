
#include "Actors/Triggers/NebulaGraphicsSkyAreaTriggerVolume.h"

#include "Actors/NebulaGraphicsSkyActor.h"
#include "Kismet/GameplayStatics.h"

ANebulaGraphicsSkyAreaTriggerVolume::ANebulaGraphicsSkyAreaTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bGenerateOverlapEventsDuringLevelStreaming = true;
}

void ANebulaGraphicsSkyAreaTriggerVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (NebulaGraphicsSkyActor)
	{
		NebulaGraphicsSkyActor->ChangeSkyArea(SkyArea);
	}
}

void ANebulaGraphicsSkyAreaTriggerVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
}

void ANebulaGraphicsSkyAreaTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	NebulaGraphicsSkyActor = Cast<ANebulaGraphicsSkyActor>(UGameplayStatics::GetActorOfClass(this, ANebulaGraphicsSkyActor::StaticClass()));
	check(NebulaGraphicsSkyActor);
}