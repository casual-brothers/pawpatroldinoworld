// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PDW_NavigationWaypoint.generated.h"

UCLASS()
class PDW_API APDW_NavigationWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDW_NavigationWaypoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AreaTag {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APDW_NavigationWaypoint* WaypointLink = {};


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
