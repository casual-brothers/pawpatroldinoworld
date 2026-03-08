#pragma once

#include "NebulaToolBaseSpawner.h"

#include "NebulaToolMovingActorsSpawner.generated.h"

USTRUCT(BlueprintType)
struct FNebulaToolMovingActorsSpawnerSetup
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawnClass = {};

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "1"))
	FPerPlatformInt ActorsToSpawn = 1;
	
	UPROPERTY(EditAnywhere)
	FTransform ActorsTransform = {};
};

UCLASS()
class NEBULATOOL_API ANebulaToolMovingActorsSpawner : public ANebulaToolBaseSpawner
{
	GENERATED_BODY()

public:

	ANebulaToolMovingActorsSpawner(const FObjectInitializer& ObjectInitializer);

	void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void SetActorsSpeed(const float InActorsSpeed);

	UFUNCTION(BlueprintCallable)
	void SetMoveActorsForward(const bool bInMoveActorsForward);

	UFUNCTION(BlueprintCallable)
	float GetActorsSpeed() const;

	UFUNCTION(BlueprintCallable)
	bool GetMoveActorsForward() const;

	UFUNCTION(BlueprintCallable)
	void GetSpawnedActorList(TArray<AActor*>& OutSpawnedActorList) const;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	TArray<FNebulaToolMovingActorsSpawnerSetup> MovingActorsSpawnerSetupList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bRandomSpawn = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (ClampMin = "0.0"), Category = "SpawnSetup")
	float ActorsDistance = 500.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (ClampMin = "0.0"), Category = "SpawnSetup")
	float ActorsSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bMoveActorsForward = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bCloseLoop = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bSnapToGround = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (bSnapToGround = "bSnapToGround", EditConditionHides), Category = "SpawnSetup")
	TEnumAsByte <ECollisionChannel> GroundCollisionChannel = ECollisionChannel::ECC_WorldStatic;

	virtual void Spawn() override;

	virtual void DestroySpawnedChildren() override;

	void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> RuntimeSpawnedActorList = {};
private:

	UPROPERTY()
	TArray<float> ActorDistanceAlongSplineList = {};
};