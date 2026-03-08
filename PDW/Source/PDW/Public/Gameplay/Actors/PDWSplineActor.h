// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "GameplayTagContainer.h"

#include "PDWSplineActor.generated.h"

class UPDWPreyComponent;

UENUM(BlueprintType)
enum class ESplinePositionRule : uint8
{
	UseDistance = 0,
	UseSplinePoint = 1,
};

USTRUCT(BlueprintType)
struct FPDWActorsList
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Obstacles {};
};

USTRUCT(BlueprintType)
struct FPDWSplineConfiguration
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag,FColor> SplineTagsAndColor {};

	UPROPERTY(EditAnywhere)
	float DistanceBetweenSplines = 0.0f;
	
	UPROPERTY(EditAnywhere)
	int32 IntermediatePoints = 5;
};

USTRUCT(BlueprintType)
struct FPDWSplineObstacleConfig
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	ESplinePositionRule PositionRules = ESplinePositionRule::UseDistance;
	UPROPERTY(EditAnywhere)
	bool bRandomizePosition = false;

	UPROPERTY(EditAnywhere)
	float ObstacleDisappearDistance = 300;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizePosition", EditConditionHides))
	FFloatRange DistanceRange;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizePosition", EditConditionHides,ClampMax=1))
	float Probability = 1.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizePosition", EditConditionHides))
	float MinStartingDistanceFromOrigin =1000.0f;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorsToSpawn {};

	UPROPERTY(EditAnywhere)
	FGameplayTag SplineTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PositionRules == ESplinePositionRule::UseDistance && !bRandomizePosition", EditConditionHides))
	float DeltaDistance = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bSkipOriginPlacement = true;
};

UCLASS()
class PDW_API APDWSplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWSplineActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void GetSplineConfiguration(FPDWSplineConfiguration& outConfig);

	UFUNCTION(CallInEditor, Category = "Utility")
	void CreateSplines();

	UFUNCTION(CallInEditor, Category = "Utility")
	void SpawnActorOnSpline();

	UFUNCTION(CallInEditor, Category = "Utility")
	void UpdateSplines();

	UFUNCTION(CallInEditor, Category = "Utility")
	void ClearSpawnedActors();

	UFUNCTION(BlueprintCallable)
	UPDWPreyComponent* SpawnPrey(TSubclassOf<AActor> inPreyToSpawn);

	TArray<AActor*> GetPlayersEndPoint()const;

	UFUNCTION(BlueprintCallable)
	void ResetObstacles();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spline")
	USplineComponent* MainSplineComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spline")
	USplineComponent* PreySplineComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FPDWSplineConfiguration SplineConfig;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TArray<FPDWSplineObstacleConfig> ObstaclesConfig;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TArray<AActor*> PlayersEndPoint = {};

private:
	
	void UpdateActorsPositions(int32 SplineIndex,USplineComponent* inSplineComponent);
	FTransform GetTransformOnSpline(USplineComponent* inSpline,bool bSkipFirstPlacement,const ESplinePositionRule& inPositionRule,ESplineCoordinateSpace::Type CoordinateSpace,int32 ActorIndex,int32 MaxActorToSpawn = 0,float SplineLenght = 0.0f, float Distance = 0.0f);
	void AdjustSpline(USplineComponent* Spline,float DeltaDistance);
	
	bool bSplinesCreated = false;
	


	UPROPERTY()
	USkeletalMeshComponent* FakeChase = nullptr;

	UPROPERTY()
	TMap<int32,FPDWActorsList> SpawnedObstacles;

	UPROPERTY()
	TMap<int32, USplineComponent*> SplineComponents;
};
