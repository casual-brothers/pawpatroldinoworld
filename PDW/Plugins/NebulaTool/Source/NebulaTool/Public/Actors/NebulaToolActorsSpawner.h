#pragma once

#include "NebulaToolBaseSpawner.h"

#include "NebulaToolActorsSpawner.generated.h"

UENUM(BlueprintType)
enum class ENebulaToolActorsSpawnerSpawnType : uint8
{
	OnSpline = 0	UMETA(DisplayName = "OnSpline"),
	OnGrid = 1		UMETA(DisplayName = "OnGrid"),
};

UCLASS()
class NEBULATOOL_API ANebulaToolActorsSpawner : public ANebulaToolBaseSpawner
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	TSubclassOf<AActor> ActorToSpawnClass = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	ENebulaToolActorsSpawnerSpawnType SpawnType = ENebulaToolActorsSpawnerSpawnType::OnSpline;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "1", EditCondition = "SpawnType == ENebulaToolActorsSpawnerSpawnType::OnSpline", EditConditionHides), Category = "SpawnSetup")
	FPerPlatformInt ActorsToSpawn = 1;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "1", EditCondition = "SpawnType == ENebulaToolActorsSpawnerSpawnType::OnGrid", EditConditionHides), Category = "SpawnSetup")
	FPerPlatformInt ActorsToSpawnOnX = 1;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "1", EditCondition = "SpawnType == ENebulaToolActorsSpawnerSpawnType::OnGrid", EditConditionHides), Category = "SpawnSetup")
	FPerPlatformInt ActorsToSpawnOnY = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (ClampMin = "0.0", EditCondition = "SpawnType == ENebulaToolActorsSpawnerSpawnType::OnGrid", EditConditionHides), Category = "SpawnSetup")
	float DistanceOnX = 250.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (ClampMin = "0.0", EditCondition = "SpawnType == ENebulaToolActorsSpawnerSpawnType::OnGrid", EditConditionHides), Category = "SpawnSetup")
	float DistanceOnY = 250.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bSnapToGround = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (bSnapToGround = "bSnapToGround", EditConditionHides), Category = "SpawnSetup")
	TEnumAsByte <ECollisionChannel> GroundCollisionChannel = ECollisionChannel::ECC_WorldStatic;

	virtual void Spawn() override;

	virtual void DestroySpawnedChildren() override;
};