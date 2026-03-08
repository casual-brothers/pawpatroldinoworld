#pragma once

#include "NebulaToolBaseSpawner.h"
#include "Engine/EngineTypes.h"

#include "NebulaToolMeshSpawner.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInstance;
class UStaticMesh;

UENUM(BlueprintType)
enum class ENebulaToolMeshSpawnerSpawnType : uint8
{
	SplinePoints = 0		UMETA(DisplayName = "SplinePoints"),
	SplineDistance = 1		UMETA(DisplayName = "SplineDistance"),
	FixedNumber = 2		UMETA(DisplayName = "FixedNumber"),
};

USTRUCT(BlueprintType)
struct FNebulaToolMeshSpawnerSetup
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	bool bFollowSplineShape = false;

	UPROPERTY(EditAnywhere)
	ENebulaToolMeshSpawnerSpawnType SpawnType = ENebulaToolMeshSpawnerSpawnType::SplinePoints;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0.0", EditCondition = "SpawnType == ENebulaToolMeshSpawnerSpawnType::SplineDistance", EditConditionHides))
	float MeshDistance = 500.0f;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "1", EditCondition = "SpawnType == ENebulaToolMeshSpawnerSpawnType::FixedNumber", EditConditionHides))
	int32 MeshToSpawn = 1;

	UPROPERTY(EditAnywhere)
	bool bUseRandomStream = false;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bUseRandomStream", EditConditionHides))
	int32 RandomStream = 0;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bFollowSplineShape", EditConditionHides))
	FVector MeshLenghtAdjustment = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FTransform Transform = {};

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bUseRandomStream", EditConditionHides))
	FTransform TransformRandom = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector);

	UPROPERTY(EditAnywhere)
	bool bSnapToGround = true;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bSnapToGround", EditConditionHides))
	TEnumAsByte<ECollisionChannel> GroundCollisionChannel = ECollisionChannel::ECC_WorldStatic;

	UPROPERTY(VisibleAnywhere)
	TArray<FSplinePoint> SplinePointList = {};

	UPROPERTY(EditAnywhere)
	UStaticMesh* StartMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<FName, UMaterialInstance*> StartMeshMaterialMap = {};

	UPROPERTY(EditAnywhere)
	UStaticMesh* MainMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<FName, UMaterialInstance*> MainMeshMaterialMap = {};

	UPROPERTY(EditAnywhere)
	UStaticMesh* EndMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<FName, UMaterialInstance*> EndMeshMaterialMap = {};

	UPROPERTY(EditAnywhere)
	FPerPlatformFloat MinDrawDistance = 0.0f;

	UPROPERTY(EditAnywhere)
	FPerPlatformFloat MaxDrawDistance = 0.0f;

	UPROPERTY(EditAnywhere)
	FPerPlatformBool CastShadow = true;

	UPROPERTY(EditAnywhere)
	bool bReceiveDecals = false;

	UPROPERTY(EditAnywhere)
	bool bCanEverAffectNavigation = true;

	UPROPERTY(EditAnywhere)
	bool bEnableCollision = true;

	UPROPERTY(EditAnywhere)
	bool bRenderCustomDepthPass = false;

	UPROPERTY(EditAnywhere)
	bool bWPORendersVelocity = false;

	UPROPERTY(EditAnywhere)
	bool bAffectDistanceFieldLighting = false;

	UPROPERTY(EditAnywhere)
	FLightingChannels LightingChannels;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEnableCollision", EditConditionHides))
	UPhysicalMaterial* PhysicalMaterial = nullptr;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEnableCollision", EditConditionHides))
	bool bBroadcastCollisionEvents = false;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEnableCollision", EditConditionHides))
	FName CollisionProfileName = FName("InvisibleWall");

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEnableCollision", EditConditionHides))
	bool bReturnMaterialOnMove = false;

	UPROPERTY()
	USplineComponent* InternalSplineComponent = nullptr;
};

UCLASS()
class NEBULATOOL_API ANebulaToolMeshSpawner : public ANebulaToolBaseSpawner
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	TArray<FNebulaToolMeshSpawnerSetup> MeshSpawnerSetupList = {};

	UFUNCTION(BlueprintNativeEvent, Category = "MeshSpawner")
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

	UFUNCTION(BlueprintNativeEvent, Category = "MeshSpawner")
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "MeshSpawner")
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	virtual void Spawn() override;

	virtual void DestroySpawnedChildren() override;

private:

	void ApplyRenderingSetup(UStaticMeshComponent* StaticMeshComponent, const FNebulaToolMeshSpawnerSetup& MeshSpawnerSetup);

	void ApplyCollisionSetup(UStaticMeshComponent* StaticMeshComponent, const FNebulaToolMeshSpawnerSetup& MeshSpawnerSetup);
};