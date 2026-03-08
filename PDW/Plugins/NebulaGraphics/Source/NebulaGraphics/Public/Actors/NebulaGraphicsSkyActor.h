#pragma once

#include "Engine/EngineTypes.h"
#include "FunctionLibraries/NebulaReflectionFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "HAL/IConsoleManager.h"
#include "Structures/NebulaGraphicsSkyActorDataAssets.h"
#include "UObject/UnrealType.h"

#include "NebulaGraphicsSkyActor.generated.h"

class UDataAsset;
class UDirectionalLightComponent;
class UExponentialHeightFogComponent;
class UMaterialInstanceDynamic;
class UMaterialParameterCollectionInstance;
class UNebulaFlowAudioManager;
class UNiagaraDataChannelAsset;
class UNiagaraDataChannelWriter;
class UPostProcessComponent;
class USkyAtmosphereComponent;
class USkyLightComponent;
class UStaticMeshComponent;
class UVolumetricCloudComponent;

/***********************************************
#TODO TEMPERATURE
	- Write on MPC
	- Constant temperature
	- Random Temperature w AllowRandom
	- Check Precipitation

#TODO CLOUDS
	- Remove VolumetricClouds component in C++
	- Manage HeterogeneusClouds
***********************************************/

USTRUCT(BlueprintType)
struct FNebulaGraphicsNiagaraDataChannelParameter
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FName ParameterName = NAME_None;

	UPROPERTY(EditAnywhere)
	int32 ParameterIndex = 0;
};

UCLASS(HideCategories = (Components, Cooking, Input, LevelInstance, Physics))
class NEBULAGRAPHICS_API ANebulaGraphicsSkyActor : public AActor
{
GENERATED_BODY()

public:

	ANebulaGraphicsSkyActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetCanUpdateSky(const bool InCanUpdateSky);

	// DAY TIME
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetDayTime(const float InDayTime);

	// TIME MULTIPLIER
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetTimeMultiplier(const float InTimeMultiplier);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetDayTimeMultiplierCurveFloat(UCurveFloat* InDayTimeMultiplierCurveFloat);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetWeatherConditionsTimeMultiplierCurveFloat(UCurveFloat* InWeatherConditionsTimeMultiplierCurveFloat);

	// WEATHER
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void CreateAndSetRandomWeatherConditionsCurves(float InStartSkyCoverPointValue, float InStartWindInstensityPointValue, FVector& InStartWindDirectionPointValue);

	// SKY COVER
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetConstantSkyCover(const float InSkyCover);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetSkyCoverCurve(UCurveFloat* InSkyCoverCurveFloat);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void CreateAndSetRandomSkyCoverCurve(float InStartSkyCoverCurvePointValue);

	// WIND
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetConstantWindIntensity(const float InWindIntensity);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetWindIntensityCurve(UCurveFloat* InWindIntensityCurveFloat);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void CreateAndSetRandomWindIntensityCurve(float InStartWindInstensityPointValue);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetConstantWindDirection(const FVector& InWindDirection);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetWindDirectionCurve(UCurveVector* InWindDirectionCurveVector);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void CreateAndSetRandomWindDirectionCurve(FVector& InStartWindDirectionPointValue);

	// PRECIPITATON
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetAllowPrecipitation(const bool InAllowPrecipitation);

	// TEMPERATURE #TODO
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void SetTemperature(const float InTemperature);

	// AREA
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	void ChangeSkyArea(const FGameplayTag& InSkyArea);

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	bool GetCanUpdateSky() const;

	// DAY TIME
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetNormalizedDayTime() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetDayTime() const;

	// TIME MULTIPLIER
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetTimeMultiplier() const;
	
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetDayTimeMultiplierCurveFloatValue(const float InTime) const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetCurrentDayTimeMultiplierCurveFloatValue() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetWeatherConditionsTimeMultiplierCurveFloatValue(const float InTime) const;
	
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetCurrentWeatherConditionsTimeMultiplierCurveFloatValue() const;

	// SKY COVER
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	bool GetAllowRandomSkyCover() const;
	
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetSkyCover() const;
	
	// WIND
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	bool GetAllowRandomWindIntensity() const;
	
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetWindIntensity() const;
	
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	bool GetAllowRandomWindDirection() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	const FVector& GetWindDirection() const;
	
	// TEMPERATURE #TODO
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetTemperature();

	// PRECIPITATON
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	bool GetAllowPrecipitation() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetStartPrecipitation() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	float GetPrecipitationIntensity() const;

	// AREA
	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	const FGameplayTag& GetCurrentArea() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	const FNebulaGraphicsSkyConfiguration& GetCurrentSkyConfiguration() const;

	UFUNCTION(BlueprintCallable, Category = "SkySetup")
	const FNebulaGraphicsSkyConfiguration& GetSkyConfiguration(const FGameplayTag& InSkyArea) const;

	virtual bool ShouldTickIfViewportsOnly() const override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	bool bCanUpdateSky = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	bool bAllowRandomSkyCover = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	bool bAllowRandomWindIntensity = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	bool bAllowRandomWindDirection = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	bool bAllowPrecipitation = false;

	// DAY TIME
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup", Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float NormalizedDayTime = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup", Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float NormalizedWeatherConditionsTime = 0.0f;

	// SKY COVER
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup", Meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = "SkyCoverCurveFloat == nullptr && !bAllowRandomSkyCover"))
	float SkyCover = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	UCurveFloat* SkyCoverCurveFloat = nullptr;

	// WIND
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup", Meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = "WindIntensityCurveFloat == nullptr && !bAllowRandomWindIntensity"))
	float WindIntensity = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	UCurveFloat* WindIntensityCurveFloat = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup", Meta = (EditCondition = "WindDirectionCurveVector == nullptr && !bAllowRandomWindDirection"))
	FVector WindDirection = { 0.0f, 1.0f, 0.0f };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	UCurveVector* WindDirectionCurveVector = nullptr;

	// TEMPERATURE #TODO
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	float Temperature = 20.0f;

	// PRECIPITATON
	UPROPERTY(VisibleAnywhere, Category = "SkySetup")
	float PrecipitationIntensity = 0.0f;

	// TIME MULTIPLIER
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|TimeMultiplier", Meta = (EditCondition = "bCanUpdateSky", EditConditionHides))
	UCurveFloat* DayTimeMultiplierCurveFloat = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|TimeMultiplier", Meta = (EditCondition = "bCanUpdateSky", EditConditionHides))
	UCurveFloat* WeatherConditionsTimeMultiplierCurveFloat = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup")
	FGameplayTag SkyArea = FGameplayTag::EmptyTag;

	// DATA ASSETS PER AREA 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|DataAssets")
	TMap<FGameplayTag, FNebulaGraphicsSkyConfiguration> SkyAreaConfigurationMap = {};

	// MPC
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	UMaterialParameterCollection* SkyMaterialParameterCollection = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCNormalizedDayTimeName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCNormalizedWeatherConditionsName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCSkyCoverName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCWindIntensityName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCWindDirectionName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCTemperatureName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCPrecipitationStartName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|MaterialParameterCollection")
	FName MPCPrecipitationIntensityName = NAME_None;

	// NDC
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|NiagaraDataChannel")
	UNiagaraDataChannelAsset* SkyNiagaraDataChannelAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|NiagaraDataChannel")
	FNebulaGraphicsNiagaraDataChannelParameter NDCPrecipitationIntensity = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|NiagaraDataChannel")
	FNebulaGraphicsNiagaraDataChannelParameter NDCWindIntensity = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkySetup|NiagaraDataChannel")
	FNebulaGraphicsNiagaraDataChannelParameter NDCWindDirection = {};

	// COMPONENTS
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SkyStaticMeshComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UDirectionalLightComponent* DirectionalLightComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	USkyAtmosphereComponent* SkyAtmosphereComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	USkyLightComponent* SkyLightComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UExponentialHeightFogComponent* ExponentialHeightFogComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UVolumetricCloudComponent* VolumetricCloudComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UPostProcessComponent* PostProcessComponent = nullptr;

	UFUNCTION(CallInEditor, Category = "SkySetup")
	void RefreshSky();

	UFUNCTION(CallInEditor, Category = "SkySetup")
	void StartSkySimulation();

	UFUNCTION(CallInEditor, Category = "SkySetup")
	void StopSkySimulation();

	UFUNCTION(CallInEditor, Category = "SkySetup")
	void RandomWeatherConditions();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Initialize();
	
	// WEATHER
	virtual void SetupRandomWeatherConditionsCurves();
	
	virtual void SetupRandomWeatherConditionsCurves(float InStartSkyCoverPointValue, float InStartWindIntensityPointValue, FVector& InStartWindDirectionPointValue);
	
	// SKY COVER
	virtual void SetupRandomSkyCoverCurve();
	
	virtual void SetupRandomSkyCoverCurve(float InStartSkyCoverPointValue);

	// WIND
	virtual void SetupRandomWindIntensityCurve();
	
	virtual void SetupRandomWindIntensityCurve(float InStartWindIntensityPointValue);

	virtual void SetupRandomWindDirectionCurve();

	virtual void SetupRandomWindDirectionCurve(FVector& InStartWindDirectionPointsValue);

	// LIGHT
	virtual void UpdateDirectionaLight();

	virtual void UpdateSkyAtmosphere();

	virtual void UpdateSkyLight();

	virtual void UpdateFog();

	virtual void UpdateVolumetricCloud();

	virtual void UpdatePostProcess();

	virtual void UpdateLightConditions();

	virtual void UpdateSkyMaterial();

	// SKY COVER
	virtual void UpdateSkyCover();

	// WIND
	virtual void UpdateWind();

	// TEMPERATURE #TODO
	virtual void UpdateTemperature();

	// PRECIPITATION
	virtual void UpdatePrecipitation();

	// WEATHER
	virtual void UpdateWeatherConditions();

	// OVERALL SKY
	virtual void UpdateSky();

private:

	bool bCanUpdateInEditor = false;

	bool bIsRealTimeReflectionCaptureEnable = false;

	bool bIsSkyAtmosphereEnable = false;

	bool bIsVolumetricFogEnable = false;

	bool bIsVolumetricCloudEnable = false;

	float DayTimeTick = 0.0f;

	float WeatherConditionsTick = 0.0f;

	float TimeMultiplier = 1.0f;

	FNebulaGraphicsSkyConfiguration CurrentSkyConfiguration = {};

	IConsoleVariable* SkylightIntensityMultiplierVar = nullptr;

	UPROPERTY()
	APlayerCameraManager* PlayerCameraManager = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* SkyMaterialInstanceDynamic = nullptr;

	UPROPERTY()
	UMaterialParameterCollectionInstance* SkyMaterialParameterCollectionInstance = nullptr;

	UPROPERTY()
	UNiagaraDataChannelWriter* NiagaraDataChannelWriter = nullptr;

	UPROPERTY()
	UNebulaFlowAudioManager* NebulaFlowAudioManager = nullptr;

	UPROPERTY()
	TArray<FNebulaReflectionPropertyData> PostProcessPropertyList = {};
};