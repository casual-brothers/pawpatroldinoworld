#pragma once

#include "Curves/CurveLinearColor.h"
#include "Engine/DataAsset.h"
#include "Engine/Scene.h"
#include "FunctionLibraries/NebulaReflectionFunctionLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "NebulaGraphicsSkyActorDataAssets.generated.h"

class USkyLightComponent;

USTRUCT(BlueprintType)
struct FNebulaGraphicsSkyCurveFloat
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "FloatCurves", Meta = (EditCondition = "bUseWeatherSystem", EditConditionHides))
	UCurveFloat* WeatherMinCurveFloat = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves", Meta = (EditCondition = "bUseWeatherSystem", EditConditionHides))
	UCurveFloat* WeatherMaxCurveFloat = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsSkyCurveLinearColor
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves", Meta = (EditCondition = "bUseWeatherSystem", EditConditionHides))
	UCurveLinearColor* WeatherMinCurveLinearColor = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves", Meta = (EditCondition = "bUseWeatherSystem", EditConditionHides))
	UCurveLinearColor* WeatherMaxCurveLinearColor = {};
};

// Directional Light
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsDirectionalLightCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat Intensity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat Temperature = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat IndirectLightingIntensity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat VolumetricScatteringIntensity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|LightShaft")
	FNebulaGraphicsSkyCurveFloat OcclusionMaskDarkness = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|LightShaft")
	FNebulaGraphicsSkyCurveFloat OcclusionDepthRange = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|LightShaft")
	FNebulaGraphicsSkyCurveFloat BloomScale = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|LightShaft")
	FNebulaGraphicsSkyCurveFloat BloomThreshold = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|LightShaft")
	FNebulaGraphicsSkyCurveFloat BloomMaxBrightness = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|Light")
	FNebulaGraphicsSkyCurveLinearColor LightColor = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|Light")
	UCurveLinearColor* LightDirection = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|LightShaft")
	FNebulaGraphicsSkyCurveLinearColor BloomTint = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|LightShaft")
	FNebulaGraphicsSkyCurveLinearColor LightShaftOverrideDirection = {};
};

// Sky Atmosphere
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsSkyAtmosphereCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// #TODO_Graphics TO BE IMPLEMENTED

};

// Sky Light
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsSkyLightCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat Intensity = {};

	// Useful to control overall lighting contrast when the real time capture is disabled
	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat IntensityMultiplier = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat IndirectLightingIntensity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|Light")
	FNebulaGraphicsSkyCurveFloat VolumetricScatteringIntensity = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|Light")
	FNebulaGraphicsSkyCurveLinearColor LightColor = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|Light")
	FNebulaGraphicsSkyCurveLinearColor LowerHemisphereColor = {};
};

// Fog
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsFogCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat FogDensity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat FogHeightFalloff = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat SecondFogDensity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat SecondFogHeightFalloff = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat SecondFogHeightOffset = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat FogMaxOpacity = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat FogStartDistance = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveFloat FogCutoffDistance = {};

	UPROPERTY(EditAnywhere, Category = "FloatCurves|VolumetricFog")
	FNebulaGraphicsSkyCurveFloat VolumetricFogViewDistance = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveLinearColor FogInscatteringColor = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|ExponentialHeightFog")
	FNebulaGraphicsSkyCurveLinearColor SkyAtmosphereAmbientContributionColorScale = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves|VolumetricFog")
	FNebulaGraphicsSkyCurveLinearColor VolumetricFogEmissive = {};
};

// Volumetric Cloud
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsVolumetricCloudCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// #TODO_Graphics TO BE IMPLEMENTED
};

// Post Process
USTRUCT(BlueprintType)
struct FPostProcessCurveFloat
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (GetOptions = "NebulaGraphics.NebulaGraphicsPostProcessCurveDataAsset.GetProperties"))
	FString PropertyName;

	UPROPERTY(EditAnywhere)
	FNebulaGraphicsSkyCurveFloat CurveFloat = {};
};

USTRUCT(BlueprintType)
struct FPostProcessCurveLinearColor
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (GetOptions = "NebulaGraphics.NebulaGraphicsPostProcessCurveDataAsset.GetProperties"))
	FString PropertyName;

	UPROPERTY(EditAnywhere)
	FNebulaGraphicsSkyCurveLinearColor CurveLinearColor = {};
};

UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsPostProcessCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "FloatCurves")
	TArray<FPostProcessCurveFloat> CurveFloatList = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves")
	TArray<FPostProcessCurveLinearColor> CurveLinearColorList = {};

private:

	UFUNCTION()
	static TArray<FString> GetProperties()
	{
		FNebulaReflectionPropertyFilter PropertyFilter;

		PropertyFilter.bExcludeNumericProperty = false;
		PropertyFilter.bExcludeBoolProperty = true;
		PropertyFilter.bExcludeNameProperty = true;
		PropertyFilter.bExcludeTextProperty = true;
		PropertyFilter.bExcludeStringProperty = true;
		PropertyFilter.bExcludeEnumProperty = true;

		PropertyFilter.bExcludeStructProperty = false;

		PropertyFilter.bExcludeVectorStruct = false;
		PropertyFilter.bExcludeRotatorStruct = false;
		PropertyFilter.bExcludeTransformStruct = false;
		PropertyFilter.bExcludeColorStruct = false;

		PropertyFilter.bExcludeMemberFromVectorStruct = true;
		PropertyFilter.bExcludeMemberFromRotatorStruct = true;
		PropertyFilter.bExcludeMemberFromTransformStruct = true;
		PropertyFilter.bExcludeMemberFromColorStruct = true;

		return UNebulaReflectionFunctionLibrary::GetStructPropertyList(FPostProcessSettings::StaticStruct(), "Settings", PropertyFilter);
	}
};

// Sky Material
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsSkyMaterialDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "FloatCurves")
	TMap<FName, FNebulaGraphicsSkyCurveFloat> CurveFloatMap = {};

	UPROPERTY(EditAnywhere, Category = "LinearColorCurves")
	TMap<FName, FNebulaGraphicsSkyCurveLinearColor> CurveLinearColorMap = {};

	UPROPERTY(EditAnywhere, Category = "Material Parameters")
	FName LightDirection = NAME_None;
};

// Weather
UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsWeatherConditionsCurveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// SKY COVER
	UPROPERTY(EditAnywhere, Category = "WeatherData")
	int32 SkyCoverCurveKeys = 10;

	UPROPERTY(EditAnywhere, Category = "WeatherData", Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float SkyCoverCurveKeyVariation = 0.25f;

	// WIND
	UPROPERTY(EditAnywhere, Category = "WeatherData")
	int32 WindIntensityCurveKeys = 15;

	UPROPERTY(EditAnywhere, Category = "WeatherData", Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float WindIntensityCurveVariation = 0.1f;

	UPROPERTY(EditAnywhere, Category = "WeatherData")
	int32 WindDirectionCurveKeys = 5;

	UPROPERTY(EditAnywhere, Category = "WeatherData", Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float WindDirectionCurveVariation = 0.2f;

	// PRECIPITATON
	UPROPERTY(EditAnywhere, Category = "WeatherData", Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float PrecipitationStart = 0.7f;

	UFUNCTION(BlueprintCallable)
	float GetRandomVariationClamped(const float InBaseValue, const float InKeyVariation)
	{
		float Value = InBaseValue + FMath::RandRange(-InKeyVariation, InKeyVariation);
		if (Value < 0.0f)
		{
			Value = 0.0f;
			Value += FMath::RandRange(0.0f, InKeyVariation);
		}
		else if (Value > 1.0f)
		{
			Value = 1.0f;
			Value -= FMath::RandRange(0.0f, InKeyVariation);
		}
		return Value;
	}
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsSkyConfiguration
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsDirectionalLightCurveDataAsset* DirectionalLightDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsSkyAtmosphereCurveDataAsset* SkyAtmosphereDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsSkyLightCurveDataAsset* SkyLightDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsFogCurveDataAsset* FogDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsVolumetricCloudCurveDataAsset* VolumetricCloudDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsPostProcessCurveDataAsset* PostProcessDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsSkyMaterialDataAsset* SkyMaterialDataAsset = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SkyConfiguration")
	UNebulaGraphicsWeatherConditionsCurveDataAsset* WeatherConditionsCurveDataAsset = nullptr;
};