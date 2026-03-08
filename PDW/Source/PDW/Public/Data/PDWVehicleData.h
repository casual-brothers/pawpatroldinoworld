#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PDWVehicleData.generated.h"


USTRUCT(BlueprintType)
struct FPDWVehicleInfo
{
	GENERATED_USTRUCT_BODY()

public:

	//Throttle
	UPROPERTY(EditDefaultsOnly, Category = "Throttle")
	float MinigameThrottleMult = 0.5f;
	// Acceleration
	UPROPERTY(EditDefaultsOnly, Category = "Acceleration")
	float EngineForce = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Acceleration")
	float MaxSpeedKPH = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Acceleration")
	float MaxReverseSpeedKPH = 20.f;

	// Brake
	UPROPERTY(EditDefaultsOnly, Category = "Brake")
	float AutoBrakeForce = 0.f;

	// Steering
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float SteeringForce = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	TObjectPtr<UCurveFloat> SteeringOverVelocity = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float MinigameSteeringMult = 5.f;
	//Gravity
	UPROPERTY(EditDefaultsOnly, Category = "Gravity")
	float GravityForce = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity")
	TObjectPtr<UCurveFloat> GravityOverTimeOnAir = nullptr;

	// Air stabilizer
	UPROPERTY(EditDefaultsOnly, Category = "Air stabilizer")
	float AirStabilizerPitchBaseForce = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Air stabilizer")
	float AirStabilizerRollBaseForce = 0.f;

	// Suspension
	UPROPERTY(EditDefaultsOnly, Category = "Suspension")
	float SuspensionLenght = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Suspension")
	float SuspensionStiffness = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Suspension")
	float SuspensionDamping = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Suspension")
	float CustomizationSuspensionLenght = 100.f;
};

UCLASS(Blueprintable, BlueprintType)
class PDW_API UPDWVehicleData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FPDWVehicleInfo DefaultData {};
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FPDWVehicleInfo> OverridePupData {};
	
};
