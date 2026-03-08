// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/PDWPersistentUser.h"
#include "Data/GameOptionsEnums.h"
#include "UI/NebulaFlowWidgetDataStructures.h"
#include "GameOptionsFunctionLibrary.generated.h"

struct FInstancedStruct;

UCLASS(meta = (BlueprintSpawnableComponent))
class PDW_API UGameOptionsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	friend class UPDWGameInstance;

public:

	UGameOptionsFunctionLibrary() {}

#pragma region Getters

public:

	static FText GetGameOptionsCategoryName(EGameOptionsCategory Category);
	static FUIAudioEvents GetGameOptionsCategoryAudio(EGameOptionsCategory Category);
	static TSoftObjectPtr<UTexture2D> GetGameOptionsCategoryImage(EGameOptionsCategory Category);
	static bool HasCategoryAnyGameOptions(EGameOptionsCategory Category);

	// Returns Game options of that category, overrode by system, if necessary and filtered by platform availability
	static void GetGameOptionsByCategory(UObject* WorldContextObject, EGameOptionsCategory Category, TArray<FInstancedStruct>& OutGameOptionsByCategory);
	// Returns Game options of that category, overrode by system, if necessary and filtered by platform availability, with setting from the save
	static void GetGameOptionsByCategoryWithSaves(UObject* WorldContextObject, EGameOptionsCategory Category, TArray<FInstancedStruct>& OutGameOptionsByCategory);

	// Returns single Game options, overrode by system, if necessary and filtered by platform availability, with setting from the save
	static void GetGameOptionsByIdWithSaves(UObject* WorldContextObject, EGameOptionsId Id, FInstancedStruct& OutGameOptions);
#pragma endregion Getters

#pragma region Value Getters and Setters

public:

	UFUNCTION(BlueprintPure)
	static bool GetCameraShakeEnabled() { return bCameraShakeEnabled; }
	
	UFUNCTION(BlueprintPure)
	static bool GetGamepadRumbleEnabled() { return bGamepadRumbleEnabled; }

	UFUNCTION(BlueprintPure)
	static float GetGamepadRumbleIntensity() { return bGamepadRumbleEnabled ? GamepadRumbleIntensity : 0.f; }

	UFUNCTION(BlueprintPure)
	static bool GetEnhancedSubtitlesEnabled() { return bEnhancedSubtitlesEnabled; }
	
	UFUNCTION(BlueprintPure)
	static bool GetEnhancedHUDEnabled() { return bEnhancedHUDEnabled; }
	
	UFUNCTION(BlueprintPure)
	static bool GetManualCameraEnabled();
	
	UFUNCTION(BlueprintPure)
	static bool GetSwapJoystickEnabled();
	
	UFUNCTION(BlueprintPure)
	static float GetJoystickDeadZone() { return JoystickDeadZone; }
	
	UFUNCTION(BlueprintPure)
	static float GetAimSensitivity() { return AimSensitivity; }
	
	UFUNCTION(BlueprintPure)
	static float GetCameraSensitivity() { return CameraSensitivity; }

protected:
	static void SetCameraShakeEnabled(const bool bEnabled) { bCameraShakeEnabled = bEnabled; }
	static void SetGamepadRumbleEnabled(const bool bEnabled) { bGamepadRumbleEnabled = bEnabled; }
	static void SetGamepadRumbleIntensity(const float Intensity) { GamepadRumbleIntensity = Intensity; }
	static void SetEnhancedSubtitlesEnabled(const bool bEnabled) { bEnhancedSubtitlesEnabled = bEnabled; }
	static void SetEnhancedHUDEnabled(const bool bEnabled) { bEnhancedHUDEnabled = bEnabled; }
	static void SetManualCameraEnabled(const bool bEnabled) { bManualCameraEnabled = bEnabled; }
	static void SetSwapJoystickEnabled(UObject* WorldContextObject, bool bEnabled);
	static void SetJoystickDeadZone(const float NewValue);
	static void SetAimSensitivity(const float NewValue) { AimSensitivity = NewValue; }
	static void SetCameraSensitivity(const float NewValue) { CameraSensitivity = NewValue; }


	static bool bCameraShakeEnabled;
	static bool bGamepadRumbleEnabled;
	static bool bEnhancedSubtitlesEnabled;
	static bool bEnhancedHUDEnabled;
	static bool bManualCameraEnabled;
	static bool bSwapJoystickEnabled;

	static float JoystickDeadZone;
	static float CameraSensitivity;
	static float AimSensitivity;
	static float GamepadRumbleIntensity;

	static bool bIsFirstLaunch;


#pragma endregion Value Getters and Setters

#pragma region Functionalities

public:

	static void LoadAllGameOptions(UObject* WorldContextObject);
	static void ChangeGameOptions(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

#pragma endregion Functionalities

#pragma region OverrideFromSystem

private:

	static void OverrideSetupGameOptionsData(UObject* WorldContextObject, FInstancedStruct& StructToOverride);

	static void OverrideResolutionSetupGameOptionsData(FInstancedStruct& StructToOverride);
	static void OverrideLanguageSetupGameOptionsData(UObject* WorldContextObject, FInstancedStruct& StructToOverride);

#pragma endregion OverrideFromSystem

#pragma region ChangeSettings

private:

	static void ApplySingleGameOptions(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

	// Game
	static void ChangeLanguage(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeVibration(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeManualCamera(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeCameraSensitivity(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeAimSensitivity(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

	// Accessibility
	static void ChangeColorDeficiency(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeCameraShake(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeJoyStickDeadZone(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeSwapJoystick(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeEnhanceSubtitles(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeEnhanceHUD(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

	// Audio
	static void ChangeMasterVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeSfxVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeMusicVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeDialoguesVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeAudioMono(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

	// Controls and Custom
	// #DEV <TBD> [#michele.b, 9 October 2025, ]
	static void ChangeRemapInput(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

	// Graphics	
	static void CheckChangeGraphicsGeneralQuality(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);

	static void ChangeGraphicsGeneralQuality(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeWindowMode(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeResolution(FInstancedStruct& GameOptionsData);
	static void ChangeAntiAliasing(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeEffects(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeFoliage(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeGamma(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeGlobalIllumination(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeLandscape(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeMaterial(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeMaxFPS(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangePostProcess(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeReflection(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeResolutionQuality(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeShading(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeShadows(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeTextures(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeViewDistance(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	static void ChangeVSync(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);
	//static void ChangeDepthOfField(UObject* WorldContextObject, FInstancedStruct& GameOptionsData);


#pragma endregion ChangeSettings
};

