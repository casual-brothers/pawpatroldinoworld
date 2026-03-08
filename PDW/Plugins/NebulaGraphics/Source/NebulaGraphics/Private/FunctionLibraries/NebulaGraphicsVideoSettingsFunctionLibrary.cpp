#include "FunctionLibraries/NebulaGraphicsVideoSettingsFunctionLibrary.h"

#include "GameFramework/GameUserSettings.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NebulaGraphics.h"
#include "RHI.h"

void UNebulaGraphicsVideoSettingsFunctionLibrary::SaveVideoSettings()
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->SaveSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ResetVideoSettings()
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ResetToCurrentSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyScreenResolution(const int32 InWidth, const int32 InHeight)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(FIntPoint(InWidth, InHeight));

		GameUserSettings->ApplyResolutionSettings(false);
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyWindowMode(const EWindowMode::Type InWindowMode)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->SetFullscreenMode(InWindowMode);

		GameUserSettings->ApplyResolutionSettings(false);
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyVSyncEnabled(const bool InVSync)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->SetVSyncEnabled(InVSync);

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyFrameRateLimit(const float InFrameRate)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->SetFrameRateLimit(InFrameRate);

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyGamma(const UObject* WorldContext, const float InGamma)
{
	UKismetSystemLibrary::ExecuteConsoleCommand(WorldContext, FString::Printf(TEXT("r.TonemapperGamma %f"), InGamma));
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyAntiAliasingQuality(const UObject* WorldContext, const int32 InAntiAliasingQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.AntiAliasingQuality = InAntiAliasingQuality;

		// TMP
		if (GameUserSettings->ScalabilityQuality.AntiAliasingQuality == 0)
		{
			// NO AA
			UKismetSystemLibrary::ExecuteConsoleCommand(WorldContext, FString::Printf(TEXT("r.AntiAliasingMethod %d"), 0));
		}
		else if (GameUserSettings->ScalabilityQuality.AntiAliasingQuality == 1)
		{
			// FXAA
			UKismetSystemLibrary::ExecuteConsoleCommand(WorldContext, FString::Printf(TEXT("r.AntiAliasingMethod %d"), 1));
		}
		else if (GameUserSettings->ScalabilityQuality.AntiAliasingQuality == 2)
		{
			// TAA
			UKismetSystemLibrary::ExecuteConsoleCommand(WorldContext, FString::Printf(TEXT("r.AntiAliasingMethod %d"), 2));
		}
		else if (GameUserSettings->ScalabilityQuality.AntiAliasingQuality == 3)
		{
			// TSR
			UKismetSystemLibrary::ExecuteConsoleCommand(WorldContext, FString::Printf(TEXT("r.AntiAliasingMethod %d"), 4));
		}

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyEffectsQuality(const UObject* WorldContext, const int32 InEffectsQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.EffectsQuality = InEffectsQuality;

		GameUserSettings->ApplyNonResolutionSettings();

		ApplyMaterialQuality(WorldContext);
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyFoliageQuality(const UObject* WorldContext, const int32 InFoliageQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.FoliageQuality = InFoliageQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyGlobalIlluminationQuality(const UObject* WorldContext, const int32 InGlobalIlluminationQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.GlobalIlluminationQuality = InGlobalIlluminationQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyLandscapeQuality(const UObject* WorldContext, const int32 InLandscapeQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.LandscapeQuality = InLandscapeQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyMaterialQuality(const UObject* WorldContext, const int32 InMaterialQuality /*= -1*/)
{
	// Update MaterialQualityLevel in order to properly manage disabled distance field
	// Currently in Unreal the quality of material is managed by EffectQuality, while distance field are managed by ShadowQuality.
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		int32 MaterialQualityLevel = 0;

		// Check if game run on Pre-GCN AMD GPU or Intel GPU or distance field is not allowed
		if (GRHIDeviceIsAMDPreGCNArchitecture ||
			IsRHIDeviceIntel() ||
			UKismetSystemLibrary::GetConsoleVariableFloatValue("r.DistanceFieldShadowing") == 0)
		{
			MaterialQualityLevel = 0;
		}
		else if (InMaterialQuality == -1)
		{
			if (GameUserSettings->ScalabilityQuality.EffectsQuality == 0)
			{
				MaterialQualityLevel = 0;	// Low
			}
			else if (GameUserSettings->ScalabilityQuality.EffectsQuality == 1)
			{
				MaterialQualityLevel = 2;	// Medium
			}
			else if (GameUserSettings->ScalabilityQuality.EffectsQuality == 2)
			{
				MaterialQualityLevel = 1;	// High
			}
			else if (GameUserSettings->ScalabilityQuality.EffectsQuality == 3)
			{
				MaterialQualityLevel = 3;	// Epic
			}
		}
		else if (InMaterialQuality == 0)
		{
			MaterialQualityLevel = 0;		// Low
		}
		else if (InMaterialQuality == 1)
		{
			MaterialQualityLevel = 2;		// Medium
		}
		else if (InMaterialQuality == 2)
		{
			MaterialQualityLevel = 1;		// High
		}
		else if (InMaterialQuality == 3)
		{
			MaterialQualityLevel = 3;		// Epic
		}

		UKismetSystemLibrary::ExecuteConsoleCommand(WorldContext, FString::Printf(TEXT("r.MaterialQualityLevel %d"), MaterialQualityLevel));
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyPostProcessQuality(const UObject* WorldContext, const int32 InPostProcessQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.PostProcessQuality = InPostProcessQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyReflectionQuality(const UObject* WorldContext, const int32 InReflectionQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.ReflectionQuality = InReflectionQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyResolutionQuality(const UObject* WorldContext, const int32 InResolutionQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.ResolutionQuality = InResolutionQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyShadingQuality(const UObject* WorldContext, const int32 InShadingQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.ShadingQuality = InShadingQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyShadowQuality(const UObject* WorldContext, const int32 InShadowQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.ShadowQuality = InShadowQuality;

		GameUserSettings->ApplyNonResolutionSettings();

		ApplyMaterialQuality(WorldContext);
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyTextureQuality(const UObject* WorldContext, const int32 InTextureQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.TextureQuality = InTextureQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyViewDistanceQuality(const UObject* WorldContext, const int32 InViewDistanceQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.ViewDistanceQuality = InViewDistanceQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyVideoQualitySettings(const UObject* WorldContext, 
																		    const int32 InAntiAliasingQuality, 
																		    const int32 InEffectsQuality, 
																		    const int32 InFoliageQuality, 
																		    const int32 InGlobalIlluminationQuality, 
																		    const int32 InLandscapeQuality, 
																		    const int32 InMaterialQuality, 
																		    const int32 InPostProcessQuality, 
																		    const int32 InReflectionQuality, 
																		    const int32 InResolutionQuality, 
																		    const int32 InShadingQuality, 
																		    const int32 InShadowQuality, 
																		    const int32 InTextureQuality, 
																		    const int32 InViewDistanceQuality)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->ScalabilityQuality.AntiAliasingQuality = InAntiAliasingQuality;
		GameUserSettings->ScalabilityQuality.EffectsQuality = InEffectsQuality;
		GameUserSettings->ScalabilityQuality.FoliageQuality = InFoliageQuality;
		GameUserSettings->ScalabilityQuality.GlobalIlluminationQuality = InGlobalIlluminationQuality;
		GameUserSettings->ScalabilityQuality.LandscapeQuality = InLandscapeQuality;
		ApplyMaterialQuality(WorldContext, InMaterialQuality);
		GameUserSettings->ScalabilityQuality.PostProcessQuality = InPostProcessQuality;
		GameUserSettings->ScalabilityQuality.ReflectionQuality = InReflectionQuality;
		GameUserSettings->ScalabilityQuality.ResolutionQuality = InResolutionQuality;
		GameUserSettings->ScalabilityQuality.ShadingQuality = InShadingQuality;
		GameUserSettings->ScalabilityQuality.ShadowQuality = InShadowQuality;
		GameUserSettings->ScalabilityQuality.TextureQuality = InTextureQuality;
		GameUserSettings->ScalabilityQuality.ViewDistanceQuality = InViewDistanceQuality;

		GameUserSettings->ApplyNonResolutionSettings();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyHardwareBenchmark(const UObject* WorldContext, const int32 InWorkScale /*= 10*/, const float InCPUMultiplier /*= 1.0f*/, const float InGPUMultiplier /*= 1.0f*/)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->RunHardwareBenchmark(InWorkScale, InCPUMultiplier, InGPUMultiplier);

		GameUserSettings->ApplyNonResolutionSettings();

		ApplyMaterialQuality(WorldContext);
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::GetDesktopResolution(FIntPoint& OutResolution)
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		OutResolution = GameUserSettings->GetDesktopResolution();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::GetCurrentScreenResolution(FIntPoint& OutResolution)
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		OutResolution = GameUserSettings->GetScreenResolution();
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::GetSupportedScreenResolutions(const int32 InMinScreenWidth, const int32 InMinScreenHeight, TArray<FIntPoint>& OutResolutionList)
{
	FScreenResolutionArray ScreenResolutionArray = {};

	if (RHIGetAvailableResolutions(ScreenResolutionArray, true))
	{
		for (const FScreenResolutionRHI& Resolution : ScreenResolutionArray)
		{
			if (Resolution.Width >= static_cast<uint32>(InMinScreenWidth) || Resolution.Height >= static_cast<uint32>(InMinScreenHeight))
			{
				FIntPoint DesktopResolution = {};
				GetDesktopResolution(DesktopResolution);

				const float DesktopAspectRatio = DesktopResolution.X / static_cast<float>(DesktopResolution.Y);
				const float ResolutionAspectRatio = Resolution.Width / static_cast<float>(Resolution.Height);

				if (FMath::IsNearlyEqual(DesktopAspectRatio, ResolutionAspectRatio, 0.1f))
				{
					OutResolutionList.AddUnique(FIntPoint(Resolution.Width, Resolution.Height));
				}
			}
		}
		if (OutResolutionList.Num() == 0)
		{
			UE_LOG(LogNebulaGraphics, Warning, TEXT("Can't find supported screen resolutions."));
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("Failed to obtain screen resolutions."));
	}
}

EWindowMode::Type UNebulaGraphicsVideoSettingsFunctionLibrary::GetWindowMode()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->GetFullscreenMode();
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return EWindowMode::Type::WindowedFullscreen;
}

bool UNebulaGraphicsVideoSettingsFunctionLibrary::IsVSyncEnabled()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->IsVSyncEnabled();
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return false;
}

float UNebulaGraphicsVideoSettingsFunctionLibrary::GetFrameRateLimit()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->GetFrameRateLimit();
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

float UNebulaGraphicsVideoSettingsFunctionLibrary::GetGamma()
{
	return UKismetSystemLibrary::GetConsoleVariableFloatValue("r.TonemapperGamma");
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetAntiAliasingQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.AntiAliasingQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetEffectsQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.EffectsQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetFoliageQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.FoliageQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetGlobalIlluminationQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.GlobalIlluminationQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetLandscapeQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.LandscapeQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetMaterialQuality()
{
	const auto MaterialQualityLevelCVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.MaterialQualityLevel"));
	return MaterialQualityLevelCVar->GetValueOnGameThread();
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetPostProcessQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.PostProcessQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetReflectionQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.ReflectionQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetResolutionQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.ResolutionQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetShadingQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.ShadingQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetShadowQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.ShadowQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetTextureQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.TextureQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

int32 UNebulaGraphicsVideoSettingsFunctionLibrary::GetViewDistanceQuality()
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		return GameUserSettings->ScalabilityQuality.ViewDistanceQuality;
	}

	UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	return INDEX_NONE;
}

void UNebulaGraphicsVideoSettingsFunctionLibrary::GetVideoQualitySettings(int32& OutAntiAliasingQuality, 
																		  int32& OutEffectsQuality, 
																		  int32& OutFoliageQuality, 
																		  int32& OutGlobalIlluminationQuality, 
																		  int32& OutLandscapeQuality, 
																	      int32& OutMaterialQuality,
																		  int32& OutPostProcessQuality, 
																		  int32& OutReflectionQuality, 
																		  int32& OutResolutionQuality, 
																		  int32& OutShadingQuality, 
																		  int32& OutShadowQuality, 
																		  int32& OutTextureQuality, 
																		  int32& OutViewDistanceQuality)
{
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		OutAntiAliasingQuality = GameUserSettings->ScalabilityQuality.AntiAliasingQuality;
		OutEffectsQuality = GameUserSettings->ScalabilityQuality.EffectsQuality;
		OutFoliageQuality = GameUserSettings->ScalabilityQuality.FoliageQuality;
		OutGlobalIlluminationQuality = GameUserSettings->ScalabilityQuality.GlobalIlluminationQuality;
		OutLandscapeQuality = GameUserSettings->ScalabilityQuality.LandscapeQuality;
		OutMaterialQuality = GetMaterialQuality();
		OutPostProcessQuality = GameUserSettings->ScalabilityQuality.PostProcessQuality;
		OutReflectionQuality = GameUserSettings->ScalabilityQuality.ReflectionQuality;
		OutResolutionQuality = GameUserSettings->ScalabilityQuality.ResolutionQuality;
		OutShadingQuality = GameUserSettings->ScalabilityQuality.ShadingQuality;
		OutShadowQuality = GameUserSettings->ScalabilityQuality.ShadowQuality;
		OutTextureQuality = GameUserSettings->ScalabilityQuality.TextureQuality;
		OutViewDistanceQuality = GameUserSettings->ScalabilityQuality.ViewDistanceQuality;
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("GameUserSettings not found."));
	}
}