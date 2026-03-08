// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FMODBlueprintStatics.h"
#include "NebulaFlowAudioFunctionLibrary.generated.h"

class UFMODEvent;

UCLASS()
class NEBULAFLOW_API UNebulaFlowAudioFunctionLibrary :  public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static FFMODEventInstance PlayUIEvent(UObject* WorldContextObject,FName EventId );
	
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static FFMODEventInstance PlayEventAtLocation(UObject* WorldContextObject, UFMODEvent* inEvent, FVector Location);

	static void PlayVoiceSpeaker(UObject* WorldContextObject, UFMODEvent* AudioEvent, FString ProgrammerSoundName = FString(""),TFunction<void()> InCallback = nullptr);

	static void ForceStopSpeaker(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static FFMODEventInstance PlaySnapShot(UObject* WorldContextObject, FName EventID);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void StopSnapShot(FFMODEventInstance Instance);

	UFUNCTION()
	static float GetAudioSurfaceValue(UObject* WorldContextObject, const AActor* inActor);
};