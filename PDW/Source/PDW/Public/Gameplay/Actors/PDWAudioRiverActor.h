// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FMODBlueprintStatics.h"
#include "PDWAudioRiverActor.generated.h"

class USplineComponent;
class UFMODEvent;

UCLASS()
class PDW_API APDWAudioRiverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWAudioRiverActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	
    virtual void OnConstruction(const FTransform& Transform) override;

	float ComputeDistance(FVector& PlayerPawnLocation);

	void ComputeDistanceAsync(FVector PlayerPawnLocation, TFunction<void(float)> OnFinished);
	
	void UpdateAudio(float Distance, FVector& Location);

	UFUNCTION(CallInEditor, Category = "Setup")
	void Debug();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USplineComponent* SplineComponent;
		
    UPROPERTY(EditAnywhere, meta=(ClampMin="5.0"), Category="Setup")
    float SplinePointsDistance = 100.0f;
	
    UPROPERTY(VisibleAnywhere)
    TArray<FVector> ComputedSplinePoints;
	
    UPROPERTY(EditAnywhere, meta = (Tooltip = "At this distance or greeater audio volume is 0"), Category="Setup")
    float MinAudioVolumeDistance = 2000.0f;

	UPROPERTY(VisibleAnywhere, Category="Setup")
	float MinAudioVolumeDistanceSquared;
	
    UPROPERTY(EditAnywhere, meta = (Tooltip = "At this distance or less audio volume is 1"), Category="Setup")
    float MaxAudioVolumeDistance = 500.0f;

	UPROPERTY(VisibleAnywhere, Category="Setup")
	float MaxAudioVolumeDistanceSquared;
		
	UPROPERTY(VisibleAnywhere)
	float MinDistanceFromPlayer1;

	UPROPERTY(VisibleAnywhere)
	float MinDistanceFromPlayer2;
		
	UPROPERTY(VisibleAnywhere)
	FVector LocationFromPlayer1;

	UPROPERTY(VisibleAnywhere)
	FVector LocationFromPlayer2;

	UPROPERTY(EditAnywhere, Category = "Setup")
	UFMODEvent* AudioEvent = nullptr;

	UPROPERTY(Transient)
	FFMODEventInstance AudioInstance;
	
	UPROPERTY()
	int32 PlayerIndex = 0;

	UPROPERTY()
	int32 CurrentVolume = 0;
	
	UPROPERTY(EditAnywhere, Category="Setup")
	bool bDebugMin;
	
	UPROPERTY(EditAnywhere, Category="Setup")
	bool bDebugMax;

};
