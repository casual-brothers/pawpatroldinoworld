// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Actors/WPBaseStreamingSourceActor.h"
#include "WPHelperSubsystem.generated.h"

DECLARE_STATS_GROUP(TEXT("UWPHelperSubsystem"), STATGROUP_WorldPartitionHelper, STATCAT_Advanced);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportEvent);

class AWPTeleportActor;
class UWPLoadingHandlerComponent;
class AWPTeleportLocation;

USTRUCT(BlueprintType)
struct FCellEntities
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<UWPLoadingHandlerComponent*> Entities = {};

	FVector CellLocation = FVector::ZeroVector;

	FName GridName = NAME_None;
};

USTRUCT(BlueprintType)
struct FWorldPartitionHelperInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName GridName = NAME_None;
	UPROPERTY()
	FBox WorldBounds = FBox();
	UPROPERTY()
	int32 CellSize = 0;
	UPROPERTY()
	float LoadingRange = 0.0f;
};

/**
 * 
 */
UCLASS()
class WORLDPARTITIONHELPER_API UWPHelperSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	static UWPHelperSubsystem* Get(UObject* WorldContext);

	FOnStreamingSourceRequested* RequestStreamingSourceAtLocation(const FVector& inLocation,const FName& StreamingSourceID);
	
	void RequestDestructionOfStreamingSource(const FName& StreamingSourceID);
	void EnableStreamingSource(const FName& StreamingSourceID);

	AWPBaseStreamingSourceActor* GetStreamingSourceActor(const FName& ID)const  {return StreamingSourcesMap[ID];};
	void RegisterStreamingSourceActor(const FName& ID, AWPBaseStreamingSourceActor* StreamingSource);
	void RemoveStreamingSourceActor(const FName& ID);

	UFUNCTION(BlueprintCallable)
	void Teleport(UPARAM(ref) TArray<AActor*>& TeleportingActors,const TArray<FTransform>& TargetLocations);
	UFUNCTION(BlueprintCallable)
	void TeleportToLocation(const FGameplayTag& TeleportLocationID,UPARAM(ref) TArray<AActor*>& inActorsToTeleport);

	UFUNCTION(BlueprintCallable)
	void SetTeleportDebug(bool bEnable);
	void RegisterEntity(UWPLoadingHandlerComponent* inLoadingComponent, const FVector& inLocation);

	UFUNCTION(BlueprintCallable)
	void RegisterTeleportLocation(const FGameplayTag& inTeleportID, AWPTeleportLocation* inTeleportLocationActor);

	UFUNCTION(BlueprintCallable)
	AWPTeleportLocation* GetTeleportActorByTag(const FGameplayTag& TeleportLocationID);

	UPROPERTY(BlueprintAssignable)
	FOnTeleportEvent OnTeleportStarted;

	UPROPERTY(BlueprintAssignable)
	FOnTeleportEvent OnTeleportCompleted;



	UPROPERTY()
	TMap<FName,FWorldPartitionHelperInfo> GridsInfo = {};

private:
	
	//TEST PERFORMANCE
	void StartCustomPerformanceTrace();
	void StopCustomPerformanceTrace();
	bool bAlreadyDone = false;
	//END TEST PERFORMANCE


	void ToggleCellQuery(const bool bInActive);
	void QueryCells();
	bool SpawnTeleportActor();
	int32 GetCellHash(const FVector& inActorLocation, const FName& inGridID);

	UPROPERTY()
	AWPTeleportActor* TeleportActor = nullptr;

	UPROPERTY()
	TMap<int32,FCellEntities> EntitiesCellMap = {};

	UPROPERTY()
	TMap<FGameplayTag, AWPTeleportLocation*> TeleportLocationsMap = {};

	UPROPERTY()
	TMap<FName,AWPBaseStreamingSourceActor*> StreamingSourcesMap ={};

	bool bStartQuery = false;

	UPROPERTY()
	FTimerHandle CellQueryTimerHandle;
};
