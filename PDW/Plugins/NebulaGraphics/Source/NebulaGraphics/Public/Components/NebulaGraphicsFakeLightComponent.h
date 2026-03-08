#pragma once

#include "Components/ActorComponent.h"

#include "NebulaGraphicsFakeLightComponent.generated.h"

class UMaterialInstance;
class UStaticMesh;

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class NEBULAGRAPHICS_API UNebulaGraphicsFakeLightComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UNebulaGraphicsFakeLightComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnRegister() override;

	virtual void OnUnregister() override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup")
	UStaticMesh* FakeLightStaticMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup")
	FTransform FakeLightTransform = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup")
	float FakeLightDrawDistance = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup")
	float FakeLightSphereInnerRadius = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup")
	float FakeLightSphereOuterRadius = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup")
	bool UseLightTemperature = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup|Material")
	UMaterialInstance* FakeLightMaterialInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup|Material")
	FName LightColorMaterialParameterName = NAME_None;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup|Material")
	FName LightCenterParameterName = NAME_None;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup|Material")
	FName LightMaxDrawDistanceParameterName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup|Material")
	FName FakeLightSphereInnerRadiusParameterName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FakeLightSetup|Material")
	FName FakeLightSphereOuterRadiusParameterName = NAME_None;

private:

	UPROPERTY()
	ULightComponent* LightComponent = nullptr;
	
	UPROPERTY()
	UStaticMeshComponent* FakeLightStaticMeshComponent = nullptr;
	
	void SetupFakeLight();
};