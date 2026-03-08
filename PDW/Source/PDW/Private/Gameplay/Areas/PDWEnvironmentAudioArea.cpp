// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Areas/PDWEnvironmentAudioArea.h"
#include "Actors/NebulaGraphicsSkyActor.h"
#include "Components/BrushComponent.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "WPHelperSubsystem.h"

#if WITH_EDITOR
#include "ToxicUtilitiesSetting.h"
#endif
#include "Engine/TargetPoint.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
	
APDWEnvironmentAudioArea::APDWEnvironmentAudioArea()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetBrushComponent()->SetCollisionProfileName(UPDWGameSettings::GetAudioEnvironmentAreaProfileName().Name);
	bGenerateOverlapEventsDuringLevelStreaming = true;
}


void APDWEnvironmentAudioArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
#if WITH_EDITOR
	if (UToxicUtilitiesSetting::Get()->bDisableAudio)
	{
		return;
	}
#endif
	UPDWAudioManager::TriggerEnvironment(this, EnvironmentType);
}
