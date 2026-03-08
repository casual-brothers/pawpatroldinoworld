// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Actors/PDWAbilitySpawnedActor.h"
#include "PDWProjectileActor.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API APDWProjectileActor : public APDWAbilitySpawnedActor
{
	GENERATED_BODY()

public:

	APDWProjectileActor();
protected:


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* ProjectileMeshComp = nullptr;
};
