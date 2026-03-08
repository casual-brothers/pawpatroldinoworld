// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WPBaseStreamingSourceActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStreamingSourceRequested);


class UWorldPartitionStreamingSourceComponent;
class UWPHelperSubsystem;

UCLASS()
class WORLDPARTITIONHELPER_API AWPBaseStreamingSourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWPBaseStreamingSourceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnStreamingCompleted();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TObjectPtr<UWorldPartitionStreamingSourceComponent> WorldPartitionStreamingSourceComponent = nullptr;

	UPROPERTY(EditAnywhere)
	bool bDebugMode = false;
	
	UPROPERTY()
	UWPHelperSubsystem* WPHelperSubsystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bManuallyDisableStreamingSource = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (EditCondition = "bManuallyDisableStreamingSource",EditConditionsHides))
	bool StartStreamingSourceOnBeginPlay = false;


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable)
	void DisableStreamingSource(bool bDestroy = true);
	
	virtual void Tick(float DeltaTime) override;
	void SetDebugMode(bool inActive) { bDebugMode = inActive; };

	FOnStreamingSourceRequested* EnableStreamingSource();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName StreamingSourceID;

	UPROPERTY(BlueprintAssignable)
	FOnStreamingSourceRequested OnStreamingSourceLoadCompleted;

private:
	
	void CalculateLoadingTime(const float inTime);
	
	float LoadingTime = 0.0f;
};
