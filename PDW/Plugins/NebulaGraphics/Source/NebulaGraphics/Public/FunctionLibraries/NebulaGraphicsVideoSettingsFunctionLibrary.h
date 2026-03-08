#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "NebulaGraphicsVideoSettingsFunctionLibrary.generated.h"

UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsVideoSettingsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Confirm and save the current video setting
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void SaveVideoSettings();

	// Reset to the original video settings
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ResetVideoSettings();

	// Apply the desired screen resolution
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyScreenResolution(const int32 InWidth, const int32 InHeight);
	
	// Apply the desired window mode
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyWindowMode(const EWindowMode::Type InWindowMode);

	// Apply the vertical sync flag
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyVSyncEnabled(const bool InVSync);

	// Apply the framerate limit
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyFrameRateLimit(const float InFrameRate);

	// Reset to the gamma
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyGamma(const UObject* WorldContext, const float InGamma);

	// Apply the AntiAliasing quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyAntiAliasingQuality(const UObject* WorldContext, const int32 InAntiAliasingQuality);

	// Apply the Effects quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyEffectsQuality(const UObject* WorldContext, const int32 InEffectsQuality);

	// Apply the Foliage quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyFoliageQuality(const UObject* WorldContext, const int32 InFoliageQuality);
	
	// Apply the GlobalIllumination quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyGlobalIlluminationQuality(const UObject* WorldContext, const int32 InGlobalIlluminationQuality);
		
	// Apply the Landscape quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyLandscapeQuality(const UObject* WorldContext, const int32 InLandscapeQuality);

	// Apply the current Material quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyMaterialQuality(const UObject* WorldContext, const int32 InMaterialQuality = -1);

	// Apply the PostProcess quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyPostProcessQuality(const UObject* WorldContext, const int32 InPostProcessQuality);
	
	// Apply the Reflection quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyReflectionQuality(const UObject* WorldContext, const int32 InReflectionQuality);
	
	// Apply the Resolution quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyResolutionQuality(const UObject* WorldContext, const int32 InResolutionQuality);
	
	// Apply the Shading quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyShadingQuality(const UObject* WorldContext, const int32 InShadingQuality);
	
	// Apply the Shadow quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyShadowQuality(const UObject* WorldContext, const int32 InShadowQuality);
	
	// Apply the Texture quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyTextureQuality(const UObject* WorldContext, const int32 InTextureQuality);
	
	// Apply the ViewDistance quality
	// 0 - Low
	// 1 - Medium
	// 2 - High
	// 3 - Epic
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyViewDistanceQuality(const UObject* WorldContext, const int32 InViewDistanceQuality);

	// Apply all video quality settings
	static void ApplyVideoQualitySettings(const UObject* WorldContext,
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
										  const int32 InViewDistanceQuality);

	// Apply all video quality settings
	// InWorkScale =  Increase this value in order to increase the system demand and accuracy of the benchmark
	// InCPUMultiplier = CPU Multiplier applied to the result of the benchmark in order to increase the apparent system demand   
	// InGPUMultiplier = GPU Multiplier applied to the result of the benchmark in order to increase the apparent system demand
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void ApplyHardwareBenchmark(const UObject* WorldContext, const int32 InWorkScale = 10, const float InCPUMultiplier = 1.0f, const float InGPUMultiplier = 1.0f);

	// Get the current screen resolution
	UFUNCTION(BlueprintPure, Category = "NebulaGraphicsVideoSettings")
	static void GetDesktopResolution(FIntPoint& OutResolution);

	// Get the current screen resolution
	UFUNCTION(BlueprintPure, Category = "NebulaGraphicsVideoSettings")
	static void GetCurrentScreenResolution(FIntPoint& OutResolution);

	// Get a list of the supported screen resolutions
	UFUNCTION(BlueprintPure, Category = "NebulaGraphicsVideoSettings")
	static void GetSupportedScreenResolutions(const int32 InMinScreenWidth, const int32 InMinScreenHeight, TArray<FIntPoint>& OutResolutionList);

	// Get the current window mode
	UFUNCTION(BlueprintPure, Category = "NebulaGraphicsVideoSettings")
	static EWindowMode::Type GetWindowMode();

	// Check whether the VSync enabled
	UFUNCTION(BlueprintPure, Category = "NebulaGraphicsVideoSettings")
	static bool IsVSyncEnabled();
	
	// Get the framerate limit
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static float GetFrameRateLimit();

	// Get the gamma
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static float GetGamma();

	// Get the AntiAliasing quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetAntiAliasingQuality();

	// Get the Effects quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetEffectsQuality();

	// Get the Foliage quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetFoliageQuality();
	
	// Get the current GlobalIllumination quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetGlobalIlluminationQuality();

	// Get the current Landscape quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetLandscapeQuality();
		
	// Get the current Material quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetMaterialQuality();

	// Get the current PostProcess quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetPostProcessQuality();
	
	// Get the current Reflection quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetReflectionQuality();
	
	// Get the current Resolution quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetResolutionQuality();
	
	// Get the current Shading quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetShadingQuality();
	
	// Get the current Shadow quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetShadowQuality();
	
	// Get the current Texture quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetTextureQuality();
	
	// Get the current ViewDistance quality
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static int32 GetViewDistanceQuality();

	// Get all current video quality settings
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsVideoSettings")
	static void GetVideoQualitySettings(int32& OutAntiAliasingQuality,
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
										int32& OutViewDistanceQuality);
};