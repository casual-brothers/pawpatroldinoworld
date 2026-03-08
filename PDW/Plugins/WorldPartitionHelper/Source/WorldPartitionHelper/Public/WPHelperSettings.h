// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/WPTeleportActor.h"
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Actors/WPBaseStreamingSourceActor.h"

#include "WPHelperSettings.generated.h"

/**
 * 
 */
UCLASS(config = WorldPartitionHelper, defaultconfig)
class WORLDPARTITIONHELPER_API UWPHelperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UWPHelperSettings(const FObjectInitializer& ObjectInitializer);

	static const UWPHelperSettings* Get() { return GetDefault<UWPHelperSettings>(); };

	UPROPERTY(Config,EditAnywhere,Category = "TeleportSettings")
	TSubclassOf<AWPTeleportActor> TeleportActorClass = nullptr;

	UPROPERTY(Config,EditAnywhere,Category = "StreamingSourceActor")
	TSubclassOf<AWPBaseStreamingSourceActor> StreamingSourceActorClass = nullptr;

	UPROPERTY(Config,EditAnywhere,Category = "WorldPartition Helper Settings")
	float CellQueryInterval = 0.2f;

	UPROPERTY(Config,EditAnywhere,Category = "WorldPartition Helper Settings")
	FName DefaultGridName = FName("MainGrid");
};
