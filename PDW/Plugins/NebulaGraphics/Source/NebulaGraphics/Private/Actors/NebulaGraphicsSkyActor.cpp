#include "Actors/NebulaGraphicsSkyActor.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveVector.h"
#include "Engine/CollisionProfile.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/NebulaFlowAudioManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "NiagaraDataChannel.h"
#include "NiagaraDataChannelAccessor.h"
#include "NiagaraDataChannelPublic.h"

ANebulaGraphicsSkyActor::ANebulaGraphicsSkyActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	SkyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkyStaticMeshComponent"));
	check(SkyStaticMeshComponent);
	SkyStaticMeshComponent->SetCanEverAffectNavigation(false);
	SkyStaticMeshComponent->SetEnableGravity(false);
	SkyStaticMeshComponent->bApplyImpulseOnDamage = false;
	SkyStaticMeshComponent->SetGenerateOverlapEvents(false);
	SkyStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SkyStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkyStaticMeshComponent->SetCastShadow(false);
	SkyStaticMeshComponent->SetAffectDynamicIndirectLighting(false);
	SkyStaticMeshComponent->SetAffectDistanceFieldLighting(false);
	SkyStaticMeshComponent->bCastDynamicShadow = false;
	SkyStaticMeshComponent->bCastStaticShadow = false;
	SkyStaticMeshComponent->SetCastContactShadow(false);
	SkyStaticMeshComponent->SetEvaluateWorldPositionOffset(false);
	SkyStaticMeshComponent->SetEvaluateWorldPositionOffsetInRayTracing(false);
	SkyStaticMeshComponent->bWorldPositionOffsetWritesVelocity = false;
	SkyStaticMeshComponent->SetReceivesDecals(false);
	SkyStaticMeshComponent->bEnableVertexColorMeshPainting = false;
	SkyStaticMeshComponent->bEnableTextureColorMeshPainting = false;
	SkyStaticMeshComponent->bEnableAutoLODGeneration = false;
	SkyStaticMeshComponent->bReceiveMobileCSMShadows = false;
	SkyStaticMeshComponent->SetupAttachment(RootComponent);

	DirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLightComponent"));
	check(DirectionalLightComponent);
	DirectionalLightComponent->SetupAttachment(RootComponent);

	SkyAtmosphereComponent = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphereComponent"));
	check(SkyAtmosphereComponent);
	SkyAtmosphereComponent->SetupAttachment(RootComponent);
	
	SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLightComponent"));
	check(SkyLightComponent);
	SkyLightComponent->SetupAttachment(RootComponent);

	ExponentialHeightFogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFogComponent"));
	check(ExponentialHeightFogComponent);
	ExponentialHeightFogComponent->SetupAttachment(RootComponent);

	VolumetricCloudComponent = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("VolumetricCloudComponent"));
	check(VolumetricCloudComponent);
	VolumetricCloudComponent->SetupAttachment(RootComponent);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	check(PostProcessComponent);
	PostProcessComponent->SetupAttachment(RootComponent);

	bEnableAutoLODGeneration = false;
	SetCanBeDamaged(false);
	bFindCameraComponentWhenViewTarget = false;
}

void ANebulaGraphicsSkyActor::SetCanUpdateSky(const bool InCanUpdateSky)
{
	bCanUpdateSky = InCanUpdateSky;
	
	if (!bCanUpdateSky)
	{
		UpdateSky();
	}
}

void ANebulaGraphicsSkyActor::SetDayTime(const float InDayTime)
{
	NormalizedDayTime = FMath::Clamp<float>(InDayTime, 0.0f, 24.0f) / 24.0f;

	if (!bCanUpdateSky)
	{
		UpdateLightConditions();
	}
}

void ANebulaGraphicsSkyActor::SetTimeMultiplier(const float InTimeMultiplier)
{
	TimeMultiplier = InTimeMultiplier >= 0.0f ? InTimeMultiplier : 0.0f;
}

void ANebulaGraphicsSkyActor::SetDayTimeMultiplierCurveFloat(UCurveFloat* InDayTimeMultiplierCurveFloat)
{
	DayTimeMultiplierCurveFloat = InDayTimeMultiplierCurveFloat;
}

void ANebulaGraphicsSkyActor::SetWeatherConditionsTimeMultiplierCurveFloat(UCurveFloat* InWeatherConditionsTimeMultiplierCurveFloat)
{
	WeatherConditionsTimeMultiplierCurveFloat = InWeatherConditionsTimeMultiplierCurveFloat;
}

void ANebulaGraphicsSkyActor::CreateAndSetRandomWeatherConditionsCurves(float InStartSkyCoverPointValue, float InStartWindInstensityPointValue, FVector& InStartWindDirectionPointValue)
{
	CreateAndSetRandomSkyCoverCurve(InStartSkyCoverPointValue);
	CreateAndSetRandomWindIntensityCurve(InStartWindInstensityPointValue);
	CreateAndSetRandomWindDirectionCurve(InStartWindDirectionPointValue);
}

void ANebulaGraphicsSkyActor::SetConstantSkyCover(const float InSkyCover)
{
	bAllowRandomSkyCover = false;

	SkyCoverCurveFloat = nullptr;

	NormalizedWeatherConditionsTime = 0.0f;
	SkyCover = FMath::Clamp<float>(InSkyCover, 0.0f, 1.0f);

	if (!bCanUpdateSky)
	{
		UpdateSky();
	}
}

void ANebulaGraphicsSkyActor::SetSkyCoverCurve(UCurveFloat* InSkyCoverCurveFloat)
{
	bAllowRandomSkyCover = false;

	SkyCoverCurveFloat = InSkyCoverCurveFloat;

	if (!bCanUpdateSky)
	{
		UpdateSky();
	}
}

void ANebulaGraphicsSkyActor::CreateAndSetRandomSkyCoverCurve(float InStartWeatherPointValue)
{
	bAllowRandomSkyCover = true;

	SetupRandomSkyCoverCurve(InStartWeatherPointValue);

	if (!bCanUpdateSky)
	{
		UpdateSky();
	}
}

void ANebulaGraphicsSkyActor::SetConstantWindIntensity(const float InWindIntensity)
{
	bAllowRandomWindIntensity = false;

	WindIntensityCurveFloat = nullptr;

	WindIntensity = FMath::Clamp<float>(InWindIntensity, 0.0f, 1.0f);

	if (!bCanUpdateSky)
	{
		UpdateWind();
	}
}

void ANebulaGraphicsSkyActor::SetWindIntensityCurve(UCurveFloat* InWindIntensityCurveFloat)
{
	bAllowRandomWindIntensity = false;

	WindIntensityCurveFloat = InWindIntensityCurveFloat;

	if (!bCanUpdateSky)
	{
		UpdateWind();
	}
}

void ANebulaGraphicsSkyActor::CreateAndSetRandomWindIntensityCurve(float InStartWindInstensityPointValue)
{
	bAllowRandomWindIntensity = true;

	SetupRandomWindIntensityCurve(InStartWindInstensityPointValue);

	if (!bCanUpdateSky)
	{
		UpdateWind();
	}
}

void ANebulaGraphicsSkyActor::SetConstantWindDirection(const FVector& InWindDirection)
{
	bAllowRandomWindDirection = false;

	WindDirectionCurveVector = nullptr;

	WindDirection = InWindDirection;

	WindDirection.Normalize();

	if (!bCanUpdateSky)
	{
		UpdateWind();
	}
}

void ANebulaGraphicsSkyActor::SetWindDirectionCurve(UCurveVector* InWindDirectionCurveVector)
{
	bAllowRandomWindDirection = false;

	WindDirectionCurveVector = InWindDirectionCurveVector;

	if (!bCanUpdateSky)
	{
		UpdateWind();
	}
}

void ANebulaGraphicsSkyActor::CreateAndSetRandomWindDirectionCurve(FVector& InStartWindDirectionPointValue)
{
	bAllowRandomWindDirection = true;

	SetupRandomWindDirectionCurve(InStartWindDirectionPointValue);

	if (!bCanUpdateSky)
	{
		UpdateWind();
	}
}

void ANebulaGraphicsSkyActor::SetAllowPrecipitation(const bool InAllowPrecipitation)
{
	bAllowPrecipitation = InAllowPrecipitation;

	if (!bCanUpdateSky)
	{
		UpdatePrecipitation();
	}
}

void ANebulaGraphicsSkyActor::SetTemperature(const float InTemperature)
{
	Temperature = InTemperature;

	if (!bCanUpdateSky)
	{
		UpdateTemperature();
	}
}

void ANebulaGraphicsSkyActor::ChangeSkyArea(const FGameplayTag& InSkyArea)
{
	SkyArea = InSkyArea;
	if (SkyAreaConfigurationMap.Contains(SkyArea))
	{
		CurrentSkyConfiguration = SkyAreaConfigurationMap[SkyArea];
	}

	if (!bCanUpdateSky)
	{
		UpdateSky();
	}
}

bool ANebulaGraphicsSkyActor::GetCanUpdateSky() const
{
	return bCanUpdateSky;
}

float ANebulaGraphicsSkyActor::GetNormalizedDayTime() const
{
	return NormalizedDayTime;
}

float ANebulaGraphicsSkyActor::GetDayTime() const
{
	return NormalizedDayTime * 24.0f;
}

float ANebulaGraphicsSkyActor::GetTimeMultiplier() const
{
	return TimeMultiplier;
}

float ANebulaGraphicsSkyActor::GetDayTimeMultiplierCurveFloatValue(const float InTime) const
{
	return DayTimeMultiplierCurveFloat ? DayTimeMultiplierCurveFloat->GetFloatValue(FMath::Clamp<float>(InTime, 0.0f, 1.0f)) : 1.0f;
}

float ANebulaGraphicsSkyActor::GetCurrentDayTimeMultiplierCurveFloatValue() const
{
	return DayTimeMultiplierCurveFloat ? DayTimeMultiplierCurveFloat->GetFloatValue(NormalizedDayTime) : 1.0f;
}

float ANebulaGraphicsSkyActor::GetWeatherConditionsTimeMultiplierCurveFloatValue(const float InTime) const
{
	return WeatherConditionsTimeMultiplierCurveFloat ? WeatherConditionsTimeMultiplierCurveFloat->GetFloatValue(FMath::Clamp<float>(InTime, 0.0f, 1.0f)) : 1.0f;
}

float ANebulaGraphicsSkyActor::GetCurrentWeatherConditionsTimeMultiplierCurveFloatValue() const
{
	return WeatherConditionsTimeMultiplierCurveFloat ? WeatherConditionsTimeMultiplierCurveFloat->GetFloatValue(NormalizedWeatherConditionsTime) : 1.0f;
}

bool ANebulaGraphicsSkyActor::GetAllowRandomSkyCover() const
{
	return bAllowRandomSkyCover;
}

float ANebulaGraphicsSkyActor::GetSkyCover()const
{
	return SkyCover;
}

bool ANebulaGraphicsSkyActor::GetAllowRandomWindIntensity() const
{
	return bAllowRandomWindIntensity;
}

float ANebulaGraphicsSkyActor::GetWindIntensity() const
{
	return WindIntensity;
}

bool ANebulaGraphicsSkyActor::GetAllowRandomWindDirection() const
{
	return bAllowRandomWindDirection;
}

const FVector& ANebulaGraphicsSkyActor::GetWindDirection() const
{
	return WindDirection;
}

float ANebulaGraphicsSkyActor::GetTemperature()
{
	return Temperature;
}

bool ANebulaGraphicsSkyActor::GetAllowPrecipitation() const
{
	return bAllowPrecipitation;
}

float ANebulaGraphicsSkyActor::GetStartPrecipitation() const
{
	return CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->PrecipitationStart;
}

float ANebulaGraphicsSkyActor::GetPrecipitationIntensity() const
{
	return PrecipitationIntensity;
}

const FGameplayTag& ANebulaGraphicsSkyActor::GetCurrentArea() const
{
	return SkyArea;
}

const FNebulaGraphicsSkyConfiguration& ANebulaGraphicsSkyActor::GetCurrentSkyConfiguration() const
{
	return CurrentSkyConfiguration;
}

const FNebulaGraphicsSkyConfiguration& ANebulaGraphicsSkyActor::GetSkyConfiguration(const FGameplayTag& InSkyArea) const
{
	if (SkyAreaConfigurationMap.Contains(SkyArea))
	{
		return SkyAreaConfigurationMap[SkyArea];
	}
	return CurrentSkyConfiguration;
}

bool ANebulaGraphicsSkyActor::ShouldTickIfViewportsOnly() const
{
	return bCanUpdateInEditor;
}

void ANebulaGraphicsSkyActor::RefreshSky()
{
	if (!bCanUpdateInEditor)
	{
		UpdateSky();
	}
}

void ANebulaGraphicsSkyActor::StartSkySimulation()
{
	bCanUpdateInEditor = true;
}

void ANebulaGraphicsSkyActor::StopSkySimulation()
{
	bCanUpdateInEditor = false;
}

void ANebulaGraphicsSkyActor::RandomWeatherConditions()
{
	SetupRandomWeatherConditionsCurves();

	UpdateSky();
}

void ANebulaGraphicsSkyActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	bCanUpdateInEditor = false;

	Initialize();

	UpdateSky();
}

void ANebulaGraphicsSkyActor::BeginPlay()
{
	Super::BeginPlay();

	Initialize();

	SetupRandomWeatherConditionsCurves();

	if (!bCanUpdateSky)
	{
		UpdateSky();
	}

	NebulaFlowAudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(this);
}

void ANebulaGraphicsSkyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if ((bCanUpdateSky || bCanUpdateInEditor) && TimeMultiplier > 0.0f)
	{
		const float DayTimeMultiplierCurveFloatValue = DayTimeMultiplierCurveFloat ? DayTimeMultiplierCurveFloat->GetFloatValue(NormalizedDayTime) : 1.0f;
		DayTimeTick += (DeltaSeconds * DayTimeMultiplierCurveFloatValue * TimeMultiplier);
		if (DayTimeTick >= 1.0f)
		{
			NormalizedDayTime += (DayTimeTick / 60.0f / 60.0f / 24.0f);
			DayTimeTick = 0.0f;

			if (NormalizedDayTime >= 1.0f)
			{
				NormalizedDayTime = 0.0f;
			}

			UpdateLightConditions();
		}

		const float WeatherConditionsTimeMultiplierCurveFloatValue = WeatherConditionsTimeMultiplierCurveFloat ? WeatherConditionsTimeMultiplierCurveFloat->GetFloatValue(NormalizedWeatherConditionsTime) : 1.0f;
		WeatherConditionsTick += (DeltaSeconds * WeatherConditionsTimeMultiplierCurveFloatValue * TimeMultiplier);
		if (WeatherConditionsTick >= 1.0f)
		{
			NormalizedWeatherConditionsTime += (WeatherConditionsTick / 60.0f / 60.0f / 24.0f);
			WeatherConditionsTick = 0.0f;

			if (NormalizedWeatherConditionsTime >= 1.0f)
			{
				NormalizedWeatherConditionsTime = 0.0f;

				SetupRandomWeatherConditionsCurves();
			}

			UpdateWeatherConditions();
		}
	}
	else
	{
		DayTimeTick = 0.0f;
		WeatherConditionsTick = 0.0f;
	}

	if (SkyNiagaraDataChannelAsset && PlayerCameraManager)
	{
		const AActor* ViewTarget = PlayerCameraManager->GetViewTarget();
		if (ViewTarget)
		{
			FNiagaraDataChannelSearchParameters PrecipitationNiagaraDataChannelSearchParameters = {};
			PrecipitationNiagaraDataChannelSearchParameters.Location = ViewTarget->GetActorLocation();
			NiagaraDataChannelWriter = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel(this, SkyNiagaraDataChannelAsset, PrecipitationNiagaraDataChannelSearchParameters, 1, true, true, true, "Sky Actor NDC");
			if (NiagaraDataChannelWriter)
			{
				NiagaraDataChannelWriter->WriteFloat(NDCPrecipitationIntensity.ParameterName, 0, PrecipitationIntensity);
				NiagaraDataChannelWriter->WriteFloat(NDCWindIntensity.ParameterName, 0, WindIntensity);
				NiagaraDataChannelWriter->WriteVector(NDCWindDirection.ParameterName, 0, WindDirection);
			}
		}
	}
}

void ANebulaGraphicsSkyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SkylightIntensityMultiplierVar = nullptr;

	PlayerCameraManager = nullptr;

	SkyMaterialInstanceDynamic = nullptr;

	SkyMaterialParameterCollectionInstance = nullptr;

	NebulaFlowAudioManager = nullptr;

	PostProcessPropertyList.Empty();

	if (bAllowRandomSkyCover)
	{
		SkyCoverCurveFloat = nullptr;
	}
	
	if (bAllowRandomWindIntensity)
	{
		WindIntensityCurveFloat = nullptr;
	}

	if (bAllowRandomWindDirection)
	{
		WindDirectionCurveVector = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ANebulaGraphicsSkyActor::Initialize()
{
	NormalizedDayTime = FMath::Clamp<float>(NormalizedDayTime, 0.0f, 1.0f);
	NormalizedWeatherConditionsTime = FMath::Clamp<float>(NormalizedWeatherConditionsTime, 0.0f, 1.0f);

	bIsRealTimeReflectionCaptureEnable = UKismetSystemLibrary::GetConsoleVariableBoolValue("r.SkyLight.RealTimeReflectionCapture");
	if (SkyLightComponent)
	{
		SkyLightComponent->SourceType = bIsRealTimeReflectionCaptureEnable ? ESkyLightSourceType::SLS_CapturedScene : ESkyLightSourceType::SLS_SpecifiedCubemap;
	}

	bIsSkyAtmosphereEnable = UKismetSystemLibrary::GetConsoleVariableBoolValue("r.SkyAtmosphere");

	bIsVolumetricFogEnable = UKismetSystemLibrary::GetConsoleVariableBoolValue("r.VolumetricFog");
	
	bIsVolumetricCloudEnable = UKismetSystemLibrary::GetConsoleVariableBoolValue("r.VolumetricCloud");

	SkylightIntensityMultiplierVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SkylightIntensityMultiplier"));
	check(SkylightIntensityMultiplierVar);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerCameraManager = PlayerController->PlayerCameraManager;
	}

	if (SkyStaticMeshComponent)
	{
		const UStaticMesh* SkyStaticMesh = SkyStaticMeshComponent->GetStaticMesh();
		if (SkyStaticMesh && Cast<UMaterialInstanceDynamic>(SkyStaticMeshComponent->GetMaterial(0)) == nullptr)
		{
			SkyMaterialInstanceDynamic = SkyStaticMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, SkyStaticMesh->GetMaterial(0));
		}
	}

	if (SkyMaterialParameterCollection)
	{
		SkyMaterialParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(CastChecked<UMaterialParameterCollection>(SkyMaterialParameterCollection));
	}
	
	if (!SkyAreaConfigurationMap.Contains(SkyArea))
	{
		TArray<FGameplayTag> AreaTagArray = {};
		SkyAreaConfigurationMap.GetKeys(AreaTagArray);
		if (!AreaTagArray.IsEmpty())
		{
			SkyArea = AreaTagArray[0];
		}
	}
	if (SkyAreaConfigurationMap.Contains(SkyArea))
	{
		CurrentSkyConfiguration = SkyAreaConfigurationMap[SkyArea];
	}

	if (CurrentSkyConfiguration.PostProcessDataAsset)
	{
		PostProcessPropertyList.Empty();
		for (auto& Elem : CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList)
		{
			FNebulaReflectionPropertyData PropertyData = {};
			UNebulaReflectionFunctionLibrary::GetPropertyByPath(PostProcessComponent, Elem.PropertyName, PropertyData);
			PostProcessPropertyList.Add(PropertyData);
		}

		for (auto& Elem : CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList)
		{
			FNebulaReflectionPropertyData PropertyData = {};
			UNebulaReflectionFunctionLibrary::GetPropertyByPath(PostProcessComponent, Elem.PropertyName, PropertyData);
			PostProcessPropertyList.Add(PropertyData);
		}
	}
}

void ANebulaGraphicsSkyActor::SetupRandomWeatherConditionsCurves()
{
	SetupRandomSkyCoverCurve();
	SetupRandomWindIntensityCurve();
	SetupRandomWindDirectionCurve();
}

void ANebulaGraphicsSkyActor::SetupRandomWeatherConditionsCurves(float InStartWeatherPointValue, float InStartWindIntensityPointValue, FVector& InStartWindDirectionPointValue)
{
	SetupRandomSkyCoverCurve(InStartWeatherPointValue);
	SetupRandomWindIntensityCurve(InStartWindIntensityPointValue);
	SetupRandomWindDirectionCurve(InStartWindDirectionPointValue);
}

void ANebulaGraphicsSkyActor::SetupRandomSkyCoverCurve()
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset && bAllowRandomSkyCover)
	{
		if (SkyCoverCurveFloat)
		{
			SetupRandomSkyCoverCurve(SkyCoverCurveFloat->FloatCurve.GetLastKey().Value);
		}
		else
		{
			SetupRandomSkyCoverCurve(FMath::RandRange(0.0f, 1.0f));
		}
	}
}

void ANebulaGraphicsSkyActor::SetupRandomSkyCoverCurve(float InStartSkyCoverPointValue)
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset && bAllowRandomSkyCover)
	{
		UCurveFloat* NewSkyCoverCurveFloat = NewObject<UCurveFloat>();
		if (NewSkyCoverCurveFloat)
		{
			FKeyHandle WeatherConditionsKeyHandle = NewSkyCoverCurveFloat->FloatCurve.AddKey(0.0f, FMath::Clamp<float>(InStartSkyCoverPointValue, 0.0f, 1.0f));
			for (int32 Index = 1; Index <= CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->SkyCoverCurveKeys; ++Index)
			{
				const float Time = Index / (float)CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->SkyCoverCurveKeys;
				const float Value = CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->GetRandomVariationClamped(NewSkyCoverCurveFloat->FloatCurve.GetKeyValue(WeatherConditionsKeyHandle), CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->SkyCoverCurveKeyVariation);

				WeatherConditionsKeyHandle = NewSkyCoverCurveFloat->FloatCurve.AddKey(Time, Value);
			}

			SkyCoverCurveFloat = NewSkyCoverCurveFloat;
		}
	}
}
		
void ANebulaGraphicsSkyActor::SetupRandomWindIntensityCurve()
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset && bAllowRandomWindIntensity)
	{
		if (WindIntensityCurveFloat)
		{
			SetupRandomWindIntensityCurve(WindIntensityCurveFloat->FloatCurve.GetLastKey().Value);
		}
		else
		{
			SetupRandomWindIntensityCurve(FMath::RandRange(0.0f, 1.0f));
		}
	}
}

void ANebulaGraphicsSkyActor::SetupRandomWindIntensityCurve(float InStartWindIntensityPointValue)
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset && bAllowRandomWindIntensity)
	{
		UCurveFloat* NewWindIntensityCurveFloat = NewObject<UCurveFloat>();
		if (NewWindIntensityCurveFloat)
		{
			FKeyHandle WindIntensityKeyHandle = NewWindIntensityCurveFloat->FloatCurve.AddKey(0.0f, InStartWindIntensityPointValue);
			for (int32 Index = 1; Index <= CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindIntensityCurveKeys; ++Index)
			{
				const float Time = Index / (float)CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindIntensityCurveKeys;
				const float Value = CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->GetRandomVariationClamped(NewWindIntensityCurveFloat->FloatCurve.GetKeyValue(WindIntensityKeyHandle), CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindIntensityCurveVariation);

				WindIntensityKeyHandle = NewWindIntensityCurveFloat->FloatCurve.AddKey(Time, Value);
			}

			WindIntensityCurveFloat = NewWindIntensityCurveFloat;
		}
	}
}

void ANebulaGraphicsSkyActor::SetupRandomWindDirectionCurve()
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset && bAllowRandomWindDirection)
	{
		FVector StartWindDirectionPointsValue = FVector::ZeroVector;
		if (WindDirectionCurveVector)
		{
			StartWindDirectionPointsValue = FVector(WindDirectionCurveVector->FloatCurves[0].GetLastKey().Value,
													WindDirectionCurveVector->FloatCurves[1].GetLastKey().Value,
													WindDirectionCurveVector->FloatCurves[2].GetLastKey().Value);
		}
		else
		{
			StartWindDirectionPointsValue = FVector(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));
		}
		SetupRandomWindDirectionCurve(StartWindDirectionPointsValue);
	}
}

void ANebulaGraphicsSkyActor::SetupRandomWindDirectionCurve(FVector& InStartWindDirectionPointsValue)
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset && bAllowRandomWindDirection)
	{
		UCurveVector* NewWindDirectionCurveVector = NewObject<UCurveVector>();
		if (NewWindDirectionCurveVector)
		{
			InStartWindDirectionPointsValue.Normalize();

			FKeyHandle WindDirectionKeyHandleX = NewWindDirectionCurveVector->FloatCurves[0].AddKey(0.0f, InStartWindDirectionPointsValue.X);
			FKeyHandle WindDirectionKeyHandleY = NewWindDirectionCurveVector->FloatCurves[1].AddKey(0.0f, InStartWindDirectionPointsValue.Y);
			FKeyHandle WindDirectionKeyHandleZ = NewWindDirectionCurveVector->FloatCurves[2].AddKey(0.0f, InStartWindDirectionPointsValue.Z);
			for (int32 Index = 1; Index <= CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindDirectionCurveKeys; ++Index)
			{
				const float Time = Index / (float)CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindDirectionCurveKeys;
				const float ValueX = CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->GetRandomVariationClamped(NewWindDirectionCurveVector->FloatCurves[0].GetKeyValue(WindDirectionKeyHandleX), CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindDirectionCurveVariation);
				const float ValueY = CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->GetRandomVariationClamped(NewWindDirectionCurveVector->FloatCurves[1].GetKeyValue(WindDirectionKeyHandleY), CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindDirectionCurveVariation);
				const float ValueZ = CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->GetRandomVariationClamped(NewWindDirectionCurveVector->FloatCurves[2].GetKeyValue(WindDirectionKeyHandleZ), CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->WindDirectionCurveVariation);

				FVector WindDirectionPointsValue = FVector(ValueX, ValueY, ValueZ);
				WindDirectionPointsValue.Normalize();

				WindDirectionKeyHandleX = NewWindDirectionCurveVector->FloatCurves[0].AddKey(Time, WindDirectionPointsValue.X);
				WindDirectionKeyHandleY = NewWindDirectionCurveVector->FloatCurves[1].AddKey(Time, WindDirectionPointsValue.Y);
				WindDirectionKeyHandleZ = NewWindDirectionCurveVector->FloatCurves[2].AddKey(Time, WindDirectionPointsValue.Z);
			}
			WindDirectionCurveVector = NewWindDirectionCurveVector;
		}
	}
}

void ANebulaGraphicsSkyActor::UpdateDirectionaLight()
{
	if (CurrentSkyConfiguration.DirectionalLightDataAsset)
	{
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->Intensity.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->Intensity.WeatherMaxCurveFloat)
		{
			const float DirectionaLightIntensity = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->Intensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->Intensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetIntensity(DirectionaLightIntensity);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->Temperature.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->Temperature.WeatherMaxCurveFloat)
		{
			const float DirectionaLightTemperature = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->Temperature.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->Temperature.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetTemperature(DirectionaLightTemperature);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->IndirectLightingIntensity.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->IndirectLightingIntensity.WeatherMaxCurveFloat)
		{
			const float DirectionaLightIndirectLightingIntensity = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->IndirectLightingIntensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->IndirectLightingIntensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetIndirectLightingIntensity(DirectionaLightIndirectLightingIntensity);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->VolumetricScatteringIntensity.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->VolumetricScatteringIntensity.WeatherMaxCurveFloat)
		{
			const float DirectionaLightVolumetricScatteringIntensity = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->VolumetricScatteringIntensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->VolumetricScatteringIntensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetVolumetricScatteringIntensity(DirectionaLightVolumetricScatteringIntensity);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionMaskDarkness.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionMaskDarkness.WeatherMaxCurveFloat)
		{
			const float DirectionaLightOcclusionMaskDarkness = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionMaskDarkness.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionMaskDarkness.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetOcclusionMaskDarkness(DirectionaLightOcclusionMaskDarkness);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionDepthRange.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionDepthRange.WeatherMaxCurveFloat)
		{
			const float DirectionaLightOcclusionDepthRange = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionDepthRange.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->OcclusionDepthRange.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetOcclusionDepthRange(DirectionaLightOcclusionDepthRange);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->BloomScale.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->BloomScale.WeatherMaxCurveFloat)
		{
			const float DirectionaLightBloomScale = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->BloomScale.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->BloomScale.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetBloomScale(DirectionaLightBloomScale);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->BloomThreshold.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->BloomThreshold.WeatherMaxCurveFloat)
		{
			const float DirectionaLightBloomThreshold = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->BloomThreshold.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->BloomThreshold.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetBloomThreshold(DirectionaLightBloomThreshold);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->BloomMaxBrightness.WeatherMinCurveFloat && CurrentSkyConfiguration.DirectionalLightDataAsset->BloomMaxBrightness.WeatherMaxCurveFloat)
		{
			const float DirectionaLightBloomMaxBrightness = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->BloomMaxBrightness.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->BloomMaxBrightness.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetBloomMaxBrightness(DirectionaLightBloomMaxBrightness);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->LightColor.WeatherMinCurveLinearColor && CurrentSkyConfiguration.DirectionalLightDataAsset->LightColor.WeatherMaxCurveLinearColor)
		{
			const FLinearColor DirectionaLightLightColor = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->LightColor.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->LightColor.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetLightColor(DirectionaLightLightColor);
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->LightDirection)
		{
			const FLinearColor DirectionaLightLinearColorValue = CurrentSkyConfiguration.DirectionalLightDataAsset->LightDirection->GetLinearColorValue(NormalizedDayTime);
			DirectionalLightComponent->SetRelativeRotation(FRotator(DirectionaLightLinearColorValue.G, DirectionaLightLinearColorValue.B, DirectionaLightLinearColorValue.R));
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->BloomTint.WeatherMinCurveLinearColor && CurrentSkyConfiguration.DirectionalLightDataAsset->BloomTint.WeatherMaxCurveLinearColor)
		{
			const FLinearColor DirectionaLightLinearColorValue = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->BloomTint.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->BloomTint.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetBloomTint(DirectionaLightLinearColorValue.ToFColor(false));
		}
		if (CurrentSkyConfiguration.DirectionalLightDataAsset->LightShaftOverrideDirection.WeatherMinCurveLinearColor && CurrentSkyConfiguration.DirectionalLightDataAsset->LightShaftOverrideDirection.WeatherMaxCurveLinearColor)
		{
			const FLinearColor DirectionaLightLinearColorValue = FMath::Lerp(CurrentSkyConfiguration.DirectionalLightDataAsset->LightShaftOverrideDirection.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.DirectionalLightDataAsset->LightShaftOverrideDirection.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			DirectionalLightComponent->SetLightShaftOverrideDirection(FVector(DirectionaLightLinearColorValue.R, DirectionaLightLinearColorValue.G, DirectionaLightLinearColorValue.B));
		}
	}
}

void ANebulaGraphicsSkyActor::UpdateSkyAtmosphere()
{
	if (CurrentSkyConfiguration.SkyAtmosphereDataAsset && bIsSkyAtmosphereEnable)
	{
		// #TODO_Graphics TO BE IMPLEMENTED
	}
}

void ANebulaGraphicsSkyActor::UpdateSkyLight()
{
	if (CurrentSkyConfiguration.SkyLightDataAsset)
	{
		if (CurrentSkyConfiguration.SkyLightDataAsset->Intensity.WeatherMinCurveFloat && CurrentSkyConfiguration.SkyLightDataAsset->Intensity.WeatherMaxCurveFloat)
		{
			const float SkyLightIntensity = FMath::Lerp(CurrentSkyConfiguration.SkyLightDataAsset->Intensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.SkyLightDataAsset->Intensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			SkyLightComponent->SetIntensity(SkyLightIntensity);
		}
		if (CurrentSkyConfiguration.SkyLightDataAsset->IntensityMultiplier.WeatherMinCurveFloat && CurrentSkyConfiguration.SkyLightDataAsset->IntensityMultiplier.WeatherMaxCurveFloat && (!bIsRealTimeReflectionCaptureEnable || !SkyLightComponent->IsRealTimeCaptureEnabled()))
		{
			const float SkyLightIntensityMultiplier = FMath::Lerp(CurrentSkyConfiguration.SkyLightDataAsset->IntensityMultiplier.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.SkyLightDataAsset->IntensityMultiplier.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			SkylightIntensityMultiplierVar->Set(SkyLightIntensityMultiplier);
		}
		if (CurrentSkyConfiguration.SkyLightDataAsset->IndirectLightingIntensity.WeatherMinCurveFloat && CurrentSkyConfiguration.SkyLightDataAsset->IndirectLightingIntensity.WeatherMaxCurveFloat)
		{
			const float SkyLightIndirectLightingIntensity = FMath::Lerp(CurrentSkyConfiguration.SkyLightDataAsset->IndirectLightingIntensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.SkyLightDataAsset->IndirectLightingIntensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			SkyLightComponent->SetIndirectLightingIntensity(SkyLightIndirectLightingIntensity);
		}
		if (CurrentSkyConfiguration.SkyLightDataAsset->VolumetricScatteringIntensity.WeatherMinCurveFloat && CurrentSkyConfiguration.SkyLightDataAsset->VolumetricScatteringIntensity.WeatherMaxCurveFloat)
		{
			const float SkyLightVolumetricScatteringIntensity = FMath::Lerp(CurrentSkyConfiguration.SkyLightDataAsset->VolumetricScatteringIntensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.SkyLightDataAsset->VolumetricScatteringIntensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			SkyLightComponent->SetVolumetricScatteringIntensity(SkyLightVolumetricScatteringIntensity);
		}
		if (CurrentSkyConfiguration.SkyLightDataAsset->LightColor.WeatherMinCurveLinearColor && CurrentSkyConfiguration.SkyLightDataAsset->LightColor.WeatherMaxCurveLinearColor)
		{
			const FLinearColor SkyLightLightColor = FMath::Lerp(CurrentSkyConfiguration.SkyLightDataAsset->LightColor.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.SkyLightDataAsset->LightColor.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			SkyLightComponent->SetLightColor(SkyLightLightColor);
		}
		if (CurrentSkyConfiguration.SkyLightDataAsset->LowerHemisphereColor.WeatherMinCurveLinearColor && CurrentSkyConfiguration.SkyLightDataAsset->LowerHemisphereColor.WeatherMaxCurveLinearColor)
		{
			const FLinearColor SkyLightLowerHemisphereColor = FMath::Lerp(CurrentSkyConfiguration.SkyLightDataAsset->LowerHemisphereColor.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.SkyLightDataAsset->LowerHemisphereColor.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			SkyLightComponent->SetLowerHemisphereColor(SkyLightLowerHemisphereColor);
		}
	}
}

void ANebulaGraphicsSkyActor::UpdateFog()
{
	if (CurrentSkyConfiguration.FogDataAsset)
	{
		if (CurrentSkyConfiguration.FogDataAsset->FogDensity.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->FogDensity.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogFogDensity = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->FogDensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->FogDensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetFogDensity(ExponentialHeightFogFogDensity);
		}
		if (CurrentSkyConfiguration.FogDataAsset->FogHeightFalloff.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->FogHeightFalloff.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogFogHeightFalloff = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->FogHeightFalloff.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->FogHeightFalloff.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetFogHeightFalloff(ExponentialHeightFogFogHeightFalloff);
		}
		if (CurrentSkyConfiguration.FogDataAsset->SecondFogDensity.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->SecondFogDensity.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogSecondFogDensity = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->SecondFogDensity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->SecondFogDensity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetSecondFogDensity(ExponentialHeightFogSecondFogDensity);
		}
		if (CurrentSkyConfiguration.FogDataAsset->SecondFogHeightFalloff.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->SecondFogHeightFalloff.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogSecondFogHeightFalloff = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->SecondFogHeightFalloff.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->SecondFogHeightFalloff.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetSecondFogHeightFalloff(ExponentialHeightFogSecondFogHeightFalloff);
		}
		if (CurrentSkyConfiguration.FogDataAsset->SecondFogHeightOffset.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->SecondFogHeightOffset.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogSecondFogHeightOffset = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->SecondFogHeightOffset.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->SecondFogHeightOffset.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetSecondFogHeightOffset(ExponentialHeightFogSecondFogHeightOffset);
		}
		if (CurrentSkyConfiguration.FogDataAsset->FogInscatteringColor.WeatherMinCurveLinearColor && CurrentSkyConfiguration.FogDataAsset->FogInscatteringColor.WeatherMaxCurveLinearColor)
		{
			const FLinearColor ExponentialHeightFogFogInscatteringColor = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->FogInscatteringColor.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->FogInscatteringColor.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetFogInscatteringColor(ExponentialHeightFogFogInscatteringColor);
		}
		if (CurrentSkyConfiguration.FogDataAsset->SkyAtmosphereAmbientContributionColorScale.WeatherMinCurveLinearColor && CurrentSkyConfiguration.FogDataAsset->SkyAtmosphereAmbientContributionColorScale.WeatherMaxCurveLinearColor)
		{
			const FLinearColor ExponentialHeightFogSkyAtmosphereAmbientContributionColorScale = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->SkyAtmosphereAmbientContributionColorScale.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->SkyAtmosphereAmbientContributionColorScale.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SkyAtmosphereAmbientContributionColorScale = ExponentialHeightFogSkyAtmosphereAmbientContributionColorScale;
		}
		if (CurrentSkyConfiguration.FogDataAsset->FogMaxOpacity.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->FogMaxOpacity.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogFogMaxOpacity = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->FogMaxOpacity.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->FogMaxOpacity.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetFogMaxOpacity(ExponentialHeightFogFogMaxOpacity);
		}
		if (CurrentSkyConfiguration.FogDataAsset->FogStartDistance.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->FogStartDistance.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogFogStartDistance = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->FogStartDistance.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->FogStartDistance.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetStartDistance(ExponentialHeightFogFogStartDistance);
		}
		if (CurrentSkyConfiguration.FogDataAsset->FogCutoffDistance.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->FogCutoffDistance.WeatherMaxCurveFloat)
		{
			const float ExponentialHeightFogFogCutoffDistance = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->FogCutoffDistance.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->FogCutoffDistance.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
			ExponentialHeightFogComponent->SetFogCutoffDistance(ExponentialHeightFogFogCutoffDistance);
		}
		if (bIsVolumetricFogEnable && ExponentialHeightFogComponent->bEnableVolumetricFog)
		{
			if (CurrentSkyConfiguration.FogDataAsset->VolumetricFogViewDistance.WeatherMinCurveFloat && CurrentSkyConfiguration.FogDataAsset->VolumetricFogViewDistance.WeatherMaxCurveFloat)
			{
				const float ExponentialHeightFogVolumetricFogViewDistance = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->VolumetricFogViewDistance.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->VolumetricFogViewDistance.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
				ExponentialHeightFogComponent->SetVolumetricFogDistance(ExponentialHeightFogVolumetricFogViewDistance);
			}
			if (CurrentSkyConfiguration.FogDataAsset->VolumetricFogEmissive.WeatherMinCurveLinearColor && CurrentSkyConfiguration.FogDataAsset->VolumetricFogEmissive.WeatherMaxCurveLinearColor)
			{
				const FLinearColor ExponentialHeightFogVolumetricFogEmissive = FMath::Lerp(CurrentSkyConfiguration.FogDataAsset->VolumetricFogEmissive.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.FogDataAsset->VolumetricFogEmissive.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
				ExponentialHeightFogComponent->SetVolumetricFogEmissive(ExponentialHeightFogVolumetricFogEmissive);
			}
		}
	}
}

void ANebulaGraphicsSkyActor::UpdateVolumetricCloud()
{
	if (CurrentSkyConfiguration.VolumetricCloudDataAsset && bIsVolumetricCloudEnable)
	{
		// #TODO_Graphics TO BE IMPLEMENTED
	}
}

void ANebulaGraphicsSkyActor::UpdatePostProcess()
{
	if (CurrentSkyConfiguration.PostProcessDataAsset)
	{
		int32 CurveFloatIndex = 0;
		int32 CurveColorIndex = 0;
		for (FNebulaReflectionPropertyData& PropertyData : PostProcessPropertyList)
		{
			if (PropertyData.Property && PropertyData.PropertyAddress)
			{
				if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(PropertyData.Property))
				{
					if (CurveFloatIndex < CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList.Num() && CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList[CurveFloatIndex].CurveFloat.WeatherMinCurveFloat && CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList[CurveFloatIndex].CurveFloat.WeatherMaxCurveFloat)
					{
						const float FloatValue = FMath::Lerp(CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList[CurveFloatIndex].CurveFloat.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList[CurveFloatIndex].CurveFloat.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
						NumericProperty->SetNumericPropertyValueFromString(PropertyData.PropertyAddress, *(FString::SanitizeFloat(FloatValue)));
						++CurveFloatIndex;
					}
				}
				else if (FStructProperty* StructProperty = CastField<FStructProperty>(PropertyData.Property))
				{
					if (CurveColorIndex < CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList.Num() && CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList[CurveColorIndex].CurveLinearColor.WeatherMinCurveLinearColor && CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList[CurveColorIndex].CurveLinearColor.WeatherMaxCurveLinearColor)
					{
						const FLinearColor LinearColorValue = FMath::Lerp(CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList[CurveColorIndex].CurveLinearColor.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList[CurveColorIndex].CurveLinearColor.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
						++CurveColorIndex;

						FProperty* PropertyR = nullptr;
						FProperty* PropertyG = nullptr;
						FProperty* PropertyB = nullptr;
						FProperty* PropertyA = nullptr;
						void* PropertyAddress = nullptr;

						if (TBaseStructure<FLinearColor>::Get() == StructProperty->Struct)
						{
							PropertyR = StructProperty->Struct->FindPropertyByName("R");
							PropertyG = StructProperty->Struct->FindPropertyByName("G");
							PropertyB = StructProperty->Struct->FindPropertyByName("B");
							PropertyA = StructProperty->Struct->FindPropertyByName("A");
						}
						else
						{
							PropertyR = StructProperty->Struct->FindPropertyByName("X");
							PropertyG = StructProperty->Struct->FindPropertyByName("Y");
							PropertyB = StructProperty->Struct->FindPropertyByName("Z");
							PropertyA = StructProperty->Struct->FindPropertyByName("W");
						}

						if (FNumericProperty* StructNumericProperty = CastField<FNumericProperty>(PropertyR))
						{
							PropertyAddress = PropertyR->ContainerPtrToValuePtr<void>(PropertyData.PropertyAddress);
							StructNumericProperty->SetNumericPropertyValueFromString(PropertyAddress, *(FString::SanitizeFloat(LinearColorValue.R)));
						}

						if (FNumericProperty* StructNumericProperty = CastField<FNumericProperty>(PropertyG))
						{
							PropertyAddress = PropertyG->ContainerPtrToValuePtr<void>(PropertyData.PropertyAddress);
							StructNumericProperty->SetNumericPropertyValueFromString(PropertyAddress, *(FString::SanitizeFloat(LinearColorValue.G)));
						}

						if (FNumericProperty* StructNumericProperty = CastField<FNumericProperty>(PropertyB))
						{
							PropertyAddress = PropertyB->ContainerPtrToValuePtr<void>(PropertyData.PropertyAddress);
							StructNumericProperty->SetNumericPropertyValueFromString(PropertyAddress, *(FString::SanitizeFloat(LinearColorValue.B)));
						}

						if (FNumericProperty* StructNumericProperty = CastField<FNumericProperty>(PropertyA))
						{
							PropertyAddress = PropertyA->ContainerPtrToValuePtr<void>(PropertyData.PropertyAddress);
							StructNumericProperty->SetNumericPropertyValueFromString(PropertyAddress, *(FString::SanitizeFloat(LinearColorValue.A)));
						}
					}
				}
			}
			else
			{
				if (CurveFloatIndex < CurrentSkyConfiguration.PostProcessDataAsset->CurveFloatList.Num())
				{
					++CurveFloatIndex;
				}
				else if (CurveColorIndex < CurrentSkyConfiguration.PostProcessDataAsset->CurveLinearColorList.Num())
				{
					++CurveColorIndex;
				}
			}
		}
	}
}

void ANebulaGraphicsSkyActor::UpdateSkyMaterial()
{
	if (CurrentSkyConfiguration.SkyMaterialDataAsset && SkyMaterialInstanceDynamic)
	{
		for (auto& Elem : CurrentSkyConfiguration.SkyMaterialDataAsset->CurveFloatMap)
		{
			if (Elem.Value.WeatherMinCurveFloat && Elem.Value.WeatherMaxCurveFloat)
			{
				const float FloatValue = FMath::Lerp(Elem.Value.WeatherMinCurveFloat->GetFloatValue(NormalizedDayTime), Elem.Value.WeatherMaxCurveFloat->GetFloatValue(NormalizedDayTime), SkyCover);
				SkyMaterialInstanceDynamic->SetScalarParameterValue(Elem.Key, FloatValue);
			}
		}

		for (auto& Elem : CurrentSkyConfiguration.SkyMaterialDataAsset->CurveLinearColorMap)
		{
			if (Elem.Value.WeatherMinCurveLinearColor && Elem.Value.WeatherMaxCurveLinearColor)
			{
				const FLinearColor LinearColorValue = FMath::Lerp(Elem.Value.WeatherMinCurveLinearColor->GetLinearColorValue(NormalizedDayTime), Elem.Value.WeatherMaxCurveLinearColor->GetLinearColorValue(NormalizedDayTime), SkyCover);
				SkyMaterialInstanceDynamic->SetVectorParameterValue(Elem.Key, LinearColorValue);
			}
		}

		SkyMaterialInstanceDynamic->SetVectorParameterValue(CurrentSkyConfiguration.SkyMaterialDataAsset->LightDirection, DirectionalLightComponent->GetForwardVector());
	}
}

void ANebulaGraphicsSkyActor::UpdateLightConditions()
{
	if (SkyMaterialParameterCollectionInstance)
	{
		SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCNormalizedDayTimeName, NormalizedDayTime);
	}

	if (NebulaFlowAudioManager)
	{
		NebulaFlowAudioManager->UpdateAudioDayNightCycle(NormalizedDayTime);
	}
	
	UpdateDirectionaLight();
	UpdateSkyAtmosphere();
	UpdateSkyLight();
	UpdateFog();
	UpdateVolumetricCloud();
	UpdatePostProcess();
	UpdateSkyMaterial();
}

void ANebulaGraphicsSkyActor::UpdateSkyCover()
{
	if (SkyCoverCurveFloat)
	{
		SkyCover = FMath::Clamp<float>(SkyCoverCurveFloat->GetFloatValue(NormalizedWeatherConditionsTime), 0.0f, 1.0f);
	}

	if (SkyMaterialParameterCollectionInstance)
	{
		SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCSkyCoverName, SkyCover);
	}
}

void ANebulaGraphicsSkyActor::UpdateWind()
{
	if (WindIntensityCurveFloat)
	{
		WindIntensity = FMath::Clamp<float>(WindIntensityCurveFloat->GetFloatValue(NormalizedWeatherConditionsTime), 0.0f, 1.0f);
	}

	if(WindDirectionCurveVector)
	{
		WindDirection = WindDirectionCurveVector->GetVectorValue(NormalizedWeatherConditionsTime);
		WindDirection.Normalize();
	}
	
	if (SkyMaterialParameterCollectionInstance)
	{
		SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCWindIntensityName, WindIntensity);
		SkyMaterialParameterCollectionInstance->SetVectorParameterValue(MPCWindDirectionName, WindDirection);
	}

	if (NebulaFlowAudioManager)
	{
		NebulaFlowAudioManager->UpdateAudioWindIntensity(WindIntensity);
	}
}

void ANebulaGraphicsSkyActor::UpdateTemperature()
{
	if (SkyMaterialParameterCollectionInstance)
	{
		SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCTemperatureName, Temperature);
	}
}

void ANebulaGraphicsSkyActor::UpdatePrecipitation()
{
	if (CurrentSkyConfiguration.WeatherConditionsCurveDataAsset)
	{
		if (bAllowPrecipitation)
		{
			PrecipitationIntensity = FMath::GetMappedRangeValueClamped(FVector2D(CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->PrecipitationStart, 1.0f), FVector2D(0.0f, 1.0f), SkyCover);
		}
		else
		{
			PrecipitationIntensity = 0.0f;
		}

		if (SkyMaterialParameterCollectionInstance)
		{
			SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCPrecipitationStartName, CurrentSkyConfiguration.WeatherConditionsCurveDataAsset->PrecipitationStart);
			SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCPrecipitationIntensityName, PrecipitationIntensity);
		}
	}

	if (NebulaFlowAudioManager)
	{
		NebulaFlowAudioManager->UpdateAudioRainIntensity(PrecipitationIntensity);
	}
}

void ANebulaGraphicsSkyActor::UpdateWeatherConditions()
{
	if (SkyMaterialParameterCollectionInstance)
	{
		SkyMaterialParameterCollectionInstance->SetScalarParameterValue(MPCNormalizedWeatherConditionsName, NormalizedWeatherConditionsTime);
	}

	// SKY COVER
	UpdateSkyCover();

	// WIND
	UpdateWind();

	// TEMPERATURE
	UpdateTemperature();

	// PRECIPITATION
	UpdatePrecipitation();
}

void ANebulaGraphicsSkyActor::UpdateSky()
{
	UpdateWeatherConditions();

	UpdateLightConditions();
}
