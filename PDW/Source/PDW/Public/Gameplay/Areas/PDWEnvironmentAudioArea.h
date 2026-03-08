// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Managers/PDWAudioManager.h"
#include "PDWEnvironmentAudioArea.generated.h"

class UPDWAudioManager;
class ANebulaGraphicsSkyActor;
class ATargetPoint;

/**
 * 
 */
UCLASS(Blueprintable)
class PDW_API APDWEnvironmentAudioArea : public ATriggerVolume
{
	GENERATED_BODY()
	
	APDWEnvironmentAudioArea();

public:

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag EnvironmentType = FGameplayTag::EmptyTag;

};
