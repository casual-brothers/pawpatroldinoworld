#pragma once

#include "NebulaToolBaseSpawner.h"

#include "NebulaToolWallCollisionSpawner.generated.h"

class UMaterialInterface;
class UPhysicalMaterial;

UCLASS()
class NEBULATOOL_API ANebulaToolWallCollisionSpawner : public ANebulaToolBaseSpawner
{
	GENERATED_BODY()

public:

	FName GetCollisionProfileName(){return CollisionProfileName; };

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (ClampMin = "0.0"), Category = "SpawnSetup")
	FVector2D WallSize = FVector2D(10.0f, 500.0f);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bSnapToGround = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (bSnapToGround = "bSnapToGround", EditConditionHides), Category = "SpawnSetup")
	TEnumAsByte <ECollisionChannel> GroundCollisionChannel = ECollisionChannel::ECC_WorldStatic;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bCanEverAffectNavigation = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	UMaterialInterface* Material = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	UPhysicalMaterial* PhysicalMaterial = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	FName CollisionProfileName = FName("InvisibleWall");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	TEnumAsByte <ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_WorldStatic;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bBroadcastCollisionEvents = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpawnSetup")
	bool bReturnMaterialOnMove = false;

	UFUNCTION(BlueprintNativeEvent, Category = "WallCollision")
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

	UFUNCTION(BlueprintNativeEvent, Category = "WallCollision")
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "WallCollision")
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	virtual void Spawn() override;

	virtual void DestroySpawnedChildren() override;
};