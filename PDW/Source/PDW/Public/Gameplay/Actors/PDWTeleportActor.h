// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/WPTeleportActor.h"
#include "PDWTeleportActor.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API APDWTeleportActor : public AWPTeleportActor
{
	GENERATED_BODY()
	
public:
	void StartTeleport(TArray<AActor*>& inTeleportingActors, const TArray<FTransform>& inPositions) override;

protected:
	void Teleport() override;

	UFUNCTION()
	void OnTransitionInEnd();

	UPROPERTY()
	TArray<FTransform> PositionsToTeleport;
};
