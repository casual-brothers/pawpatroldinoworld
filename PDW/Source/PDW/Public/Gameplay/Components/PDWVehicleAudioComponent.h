// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "Managers/PDWAudioManager.h"
#include "PDWVehicleAudioComponent.generated.h"

class APDWVehiclePawn;
class UPDWAudioManager;

UENUM(BlueprintType)
enum class EAudioType :uint8
{
	Engine,
	Wheels,
	Reverse,
	Screech,
};


USTRUCT(BlueprintType)
struct FVehicleComponentAudioStruct : public FSimpleAudioStruct
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bOneShotSound = false;
	
	UPROPERTY()
	UFMODAudioComponent* AudioComponentInstance = nullptr;
};

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PDW_API UPDWVehicleAudioComponent : public UFMODAudioComponent
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;
	void InitializeAudioComponents();

	UFUNCTION()
	void OnVehicleCollision(float SpeedKPH, EPhysicalSurface Surface, FVector HitLocation);

	UFUNCTION()
	void OnGroundSurfaceChange(FName NewSurfaceName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<EAudioType, FVehicleComponentAudioStruct> AudioConfigurations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float DriftTreshold = 2.0f;

	UFUNCTION()
	void ResetImpactTimer();

private:

	EPhysicalSurface LastSurface;
	
	UPROPERTY()
	APDWVehiclePawn* VehicleOwner = nullptr;

	UPROPERTY()
	UPDWAudioManager* AudioMgr = nullptr;

	bool bImpactTimerOn = false;
	bool bMoving = false;
	bool bScreeching = false;

	FTimerHandle ImpactTimerHandle;
public:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
