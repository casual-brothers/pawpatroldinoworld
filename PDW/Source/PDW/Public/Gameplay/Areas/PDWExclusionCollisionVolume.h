// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "GameplayTagContainer.h"
#include "PDWExclusionCollisionVolume.generated.h"


UCLASS()
class PDW_API APDWExclusionCollisionVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
	APDWExclusionCollisionVolume();

public:

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;
};
