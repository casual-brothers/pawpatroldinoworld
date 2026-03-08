// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PDWPreyComponent.generated.h"

class USplineComponent;

USTRUCT(BlueprintType)
struct FPDWPreyConfiguration
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxSpeed = 600.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MinSpeed = 100.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxDistanceFromPlayer = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistanceFromPlayer = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedAdjustRate = 2.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWPreyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPDWPreyComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPlayerPawn(APawn* inPawn);
	void SetSpline(USplineComponent* inSpline);

	UFUNCTION(BlueprintCallable)
	void PauseLogic(bool bInPause);
protected:

	virtual void BeginPlay() override;

	void MoveAlongSpline(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FPDWPreyConfiguration PreyConfig;
	
	UPROPERTY()
	APawn* PlayerPawn = nullptr;

private:

	float CurrentSpeed = 0.f; 
	float DistanceAlongSpline = 0.f;
	bool bPauseLogic = false;

	UPROPERTY()
	USplineComponent* Spline = nullptr;
};
