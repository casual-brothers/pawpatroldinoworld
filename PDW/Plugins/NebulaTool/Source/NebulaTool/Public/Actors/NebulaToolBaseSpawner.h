#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"

#include "NebulaToolBaseSpawner.generated.h"

UCLASS(Abstract)
class NEBULATOOL_API ANebulaToolBaseSpawner : public AActor
{
	GENERATED_BODY()

public:

	ANebulaToolBaseSpawner(const FObjectInitializer& ObjectInitializer);

	virtual void Destroyed() override;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostEditMove(bool bFinished);

#endif

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spline")
	USplineComponent* SplineComponent = nullptr;

	virtual void Spawn();

	virtual void DestroySpawnedChildren();
};