// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "PDWMinigameCameraComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PDW_API UPDWMinigameCameraComponent : public UCameraComponent, public IWorldPartitionStreamingSourceProvider
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldPartition)
	uint32 bEnableStreamingSource:1;

	/** Whether the PlayerController streaming source should activate cells after loading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldPartition, meta=(EditCondition="bEnableStreamingSource"))
	uint32 bStreamingSourceShouldActivate:1;

	/** Whether the PlayerController streaming source should block on slow streaming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldPartition, meta=(EditCondition="bEnableStreamingSource"))
	uint32 bStreamingSourceShouldBlockOnSlowStreaming:1;

	/** PlayerController streaming source priority. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldPartition, meta=(EditCondition="bEnableStreamingSource"))
	EStreamingSourcePriority StreamingSourcePriority;

	/** Color used for debugging. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldPartition, meta = (EditCondition = "bEnableStreamingSource"))
	FColor StreamingSourceDebugColor;

	/** Optional aggregated shape list used to build a custom shape for the streaming source. When empty, fallbacks sphere shape with a radius equal to grid's loading range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldPartition, meta = (EditCondition = "bEnableStreamingSource"))
	TArray<FStreamingSourceShape> StreamingSourceShapes;

	bool GetStreamingSource(FWorldPartitionStreamingSource& StreamingSource) const override;


	bool GetStreamingSources(TArray<FWorldPartitionStreamingSource>& OutStreamingSources) const override;


	UFUNCTION(BlueprintCallable, Category = WorldPartition)
	virtual bool IsStreamingSourceEnabled() const { return bEnableStreamingSource; }

	/**
	* Whether the PlayerController streaming source should activate cells after loading.
	* Default implementation returns bStreamingSourceShouldActivate but can be overriden in child classes.
	* @return true if it should.
	*/
	UFUNCTION(BlueprintCallable, Category = WorldPartition)
	virtual bool StreamingSourceShouldActivate() const { return bEnableStreamingSource && bStreamingSourceShouldActivate; }

	/**
	* Whether the PlayerController streaming source should block on slow streaming.
	* Default implementation returns bStreamingSourceShouldBlockOnSlowStreaming but can be overriden in child classes.
	* @return true if it should.
	*/
	UFUNCTION(BlueprintCallable, Category = WorldPartition)
	virtual bool StreamingSourceShouldBlockOnSlowStreaming() const { return bEnableStreamingSource && bStreamingSourceShouldBlockOnSlowStreaming; }

	/**
	* Gets the streaming source priority.
	* Default implementation returns StreamingSourcePriority but can be overriden in child classes.
	* @return the streaming source priority.
	*/
	UFUNCTION(BlueprintCallable, Category = WorldPartition)
	virtual EStreamingSourcePriority GetStreamingSourcePriority() const { return StreamingSourcePriority; }

	void OnRegister() override;
	void OnUnregister() override;
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

protected:

	UFUNCTION(BlueprintCallable, Category = WorldPartition)
	virtual void GetStreamingSourceLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const;

	void GetStreamingSourceShapes(TArray<FStreamingSourceShape>& OutShapes) const;
	virtual bool GetStreamingSourcesInternal(TArray<FWorldPartitionStreamingSource>& OutStreamingSources) const;
};
