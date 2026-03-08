// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PDWSplineObstacles.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API APDWSplineObstacles : public AActor
{
	GENERATED_BODY()

public:

	APDWSplineObstacles();

	void SetSplineOwner(USplineComponent* inSplineOwner);

#if WITH_EDITOR

	void PostEditMove(bool bFinished) override;

#endif

protected:

	UPROPERTY()
	TWeakObjectPtr<USplineComponent> SplineOwner = nullptr;
	
private:

	float CurrentPositionOnSpline = 0.0f;
};
