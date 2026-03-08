// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Gameplay/Areas/PDWEnvironmentSkyArea.h"
#include "Actors/NebulaGraphicsSkyActor.h"
#include "Components/BrushComponent.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"


APDWEnvironmentSkyArea::APDWEnvironmentSkyArea()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetBrushComponent()->SetCollisionProfileName(UPDWGameSettings::GetAudioEnvironmentAreaProfileName().Name);
	bGenerateOverlapEventsDuringLevelStreaming = true;
}


void APDWEnvironmentSkyArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (bIgnoreInMultiplayer && UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		return;
	}
	if (SkyActor)
	{
		SkyActor->ChangeSkyArea(EnvironmentType);
	}

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this);
	if (EventSubsystem)
	{	
		EventSubsystem->OnLocationEnter.Broadcast(WaypointType);
	}
}

void APDWEnvironmentSkyArea::BeginPlay()
{
	Super::BeginPlay();

	SkyActor = Cast<ANebulaGraphicsSkyActor>(UGameplayStatics::GetActorOfClass(this, ANebulaGraphicsSkyActor::StaticClass()));

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	if (GM)
	{
		GM->SkyAreas.AddUnique(this);
	}
}