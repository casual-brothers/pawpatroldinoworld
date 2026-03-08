// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "FunctionLibraries/NebulaGraphicsVideoSettingsFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Data/GameOptionsData.h"
#include "Data/GameOptionsDeveloperSettings.h"
#include "FunctionLibraries/NebulaFlowLocaleFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Engine/GameInstance.h"
#include "PDWGameInstance.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "Data/PDWInputsData.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableKeySettings.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"
#include "UI/Core/PDWNavigationConfig.h"

#pragma region Getters

FText UGameOptionsFunctionLibrary::GetGameOptionsCategoryName(EGameOptionsCategory Category)
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (!GameOptionsDevSet->GameOptionsData.Contains(Category))
	{
		return FText();
	}
	return GameOptionsDevSet->GameOptionsData[Category].GameOptionsCategoryName;
}

FUIAudioEvents UGameOptionsFunctionLibrary::GetGameOptionsCategoryAudio(EGameOptionsCategory Category)
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (!GameOptionsDevSet->GameOptionsData.Contains(Category))
	{
		return FUIAudioEvents();
	}
	return GameOptionsDevSet->GameOptionsData[Category].UIAudioEvents;
}

TSoftObjectPtr<UTexture2D> UGameOptionsFunctionLibrary::GetGameOptionsCategoryImage(EGameOptionsCategory Category)
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (!GameOptionsDevSet->GameOptionsData.Contains(Category))
	{
		return nullptr;
	}
	return GameOptionsDevSet->GameOptionsData[Category].CategoryImage;
}

bool UGameOptionsFunctionLibrary::HasCategoryAnyGameOptions(EGameOptionsCategory Category)
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (!GameOptionsDevSet->GameOptionsData.Contains(Category))
	{
		return false;
	}
	const FGameOptionsCategory* GameSettingsCategory = &(GameOptionsDevSet->GameOptionsData[Category]);

	bool bHasSettings = false;
	for (auto SingleSettings : GameSettingsCategory->GameOptions)
	{
		if (!SingleSettings.IsValid() || !SingleSettings.GetScriptStruct()->IsChildOf(FGameOptionsBaseData::StaticStruct()))
		{
			continue;
		}
		const FGameOptionsBaseData& SingleSettingsData = SingleSettings.GetMutable<FGameOptionsBaseData>();
		if (SingleSettingsData.GetIsAvailableInCurrentPlatform())
		{
			return true;
		}
	}
	return false;
}

void UGameOptionsFunctionLibrary::GetGameOptionsByCategory(UObject* WorldContextObject, EGameOptionsCategory Category, TArray<FInstancedStruct>& OutGameOptionsByCategory)
{
	// Get data from the config
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (!GameOptionsDevSet->GameOptionsData.Contains(Category))
	{
		return;
	}
	const FGameOptionsCategory& CurrentCategoryData = GameOptionsDevSet->GameOptionsData[Category];

	for (FInstancedStruct GameOptions : CurrentCategoryData.GameOptions)
	{
		FGameOptionsBaseData* BaseGameOptionsData = GameOptions.GetMutablePtr<FGameOptionsBaseData>();

		// filter out unavailable options for the current platform
		if (!BaseGameOptionsData->GetIsAvailableInCurrentPlatform())
		{
			continue;
		}
		if (!ensureMsgf(BaseGameOptionsData->GetId() != EGameOptionsId::E_None, TEXT("in category %s there is a game option without ID, it will be left out"), *UEnum::GetValueAsString(Category)))
		{
			continue;
		}
		if (WorldContextObject)
		{
			// override by system
			OverrideSetupGameOptionsData(WorldContextObject, GameOptions);
		}
		// add
		OutGameOptionsByCategory.Add(GameOptions);
	}
}

void UGameOptionsFunctionLibrary::GetGameOptionsByCategoryWithSaves(UObject* WorldContextObject, EGameOptionsCategory Category, TArray<FInstancedStruct>& GameOptionsByCategory)
{
	GetGameOptionsByCategory(WorldContextObject, Category, GameOptionsByCategory);

	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(WorldContextObject);
	const auto SavedGameOptions = PersistentUser->GetGameOptions();


	for (FInstancedStruct& GameOptions : GameOptionsByCategory)
	{
		FGameOptionsBaseData* BaseGameOptionsData = GameOptions.GetMutablePtr<FGameOptionsBaseData>();

		// override current value from saves
		if (SavedGameOptions.Contains(BaseGameOptionsData->GetId()))
		{
			BaseGameOptionsData->SetCurrentValueIndex(SavedGameOptions[BaseGameOptionsData->GetId()]);
		}
	}

	return;
}

void UGameOptionsFunctionLibrary::GetGameOptionsByIdWithSaves(UObject* WorldContextObject, EGameOptionsId Id, FInstancedStruct& OutGameOptions)
{
	// for each category
	for (uint8 i = 1; i < static_cast<uint8>(EGameOptionsCategory::MAX); i++)
	{
		EGameOptionsCategory Category = static_cast<EGameOptionsCategory>(i);

		//for each game options available in this platform
		TArray<FInstancedStruct> CategoryGameOptions;
		GetGameOptionsByCategoryWithSaves(WorldContextObject, Category, CategoryGameOptions);
		for (FInstancedStruct& GameOptions : CategoryGameOptions)
		{
			FGameOptionsBaseData* BaseGameOptionsData = GameOptions.GetMutablePtr<FGameOptionsBaseData>();
			if (BaseGameOptionsData->GetId() == Id)
			{
				OutGameOptions = GameOptions;
				return;
			}
		}
	}
}

#pragma endregion Getters

#pragma region Value Getters and Setters

bool UGameOptionsFunctionLibrary::GetManualCameraEnabled()
{
#if PLATFORM_SWITCH
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0))
	{
		return false;
	}
#endif

	return bManualCameraEnabled;
}


bool UGameOptionsFunctionLibrary::GetSwapJoystickEnabled()
{
#if PLATFORM_SWITCH
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0))
	{
		return false;
	}
#endif
	return bSwapJoystickEnabled;
}

void UGameOptionsFunctionLibrary::SetSwapJoystickEnabled(UObject* WorldContextObject, bool bEnabled)
{
	bSwapJoystickEnabled = bEnabled;
	
	// If we have a single joy-con, we treat this as if it is disabled
#if PLATFORM_SWITCH
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0))
	{
		bEnabled = false;
	}
#endif

	UPDWInputsData* InputsData = GetDefault<UGameOptionsDeveloperSettings>()->DefaultInputData.LoadSynchronous();
	TArray<UInputMappingContext*>& AllIMCs = InputsData->AllIMCs;

	const FKey LeftStickX = EKeys::Gamepad_LeftX;
	const FKey LeftStickY = EKeys::Gamepad_LeftY;
	const FKey LeftStick2D = EKeys::Gamepad_Left2D;
	const FKey RightStickX = EKeys::Gamepad_RightX;
	const FKey RightStickY = EKeys::Gamepad_RightY;
	const FKey RightStick2D = EKeys::Gamepad_Right2D;

	TMap<FKey, FKey> FromTo = TMap<FKey, FKey>{};
	FromTo.Add(LeftStickX, RightStickX);
	FromTo.Add(LeftStickY, RightStickY);
	FromTo.Add(LeftStick2D, RightStick2D);
	FromTo.Add(RightStickX, LeftStickX);
	FromTo.Add(RightStickY, LeftStickY);
	FromTo.Add(RightStick2D, LeftStick2D);

	ULocalPlayer* LocalPlayer = UGameplayStatics::GetGameInstance(WorldContextObject)->GetFirstGamePlayer();
	if (!ensure(LocalPlayer)) return;

	UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!ensure(EISubsystem)) return;

	UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();
	if (!ensure(UserSettings)) return;

	for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : UserSettings->GetAllAvailableKeyProfiles())
	{
		const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

		for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows()) // will be only 1 for most of our games
		{
			// Create a setting row for anything with valid mappings and that we haven't created yet
			if (RowPair.Value.HasAnyMappings())
			{
				for (auto it = RowPair.Value.Mappings.begin(); it != RowPair.Value.Mappings.end(); ++it)
				{
					const FKey& DefaultKey = it->GetDefaultKey();
					if (FromTo.Contains(DefaultKey))
					{
						FMapPlayerKeyArgs NewArgs = {};
						NewArgs.MappingName = it->GetMappingName();
						NewArgs.Slot = it->GetSlot();
						NewArgs.NewKey = bEnabled ? FromTo[DefaultKey] : DefaultKey;

						FGameplayTagContainer FailureReason;
						UserSettings->MapPlayerKey(NewArgs, FailureReason);
					}
				}
			}
		}
	}


	UserSettings->AsyncSaveSettings();
	FNebulaFlowCoreDelegates::OnActionRemapped.Broadcast();


	// Navigation
	TSharedPtr<FNavigationConfig> CurrentNavConfig = FSlateApplication::Get().GetNavigationConfig();

	TSharedPtr<FCustomNavigationConfig> MyCustomConfig = StaticCastSharedPtr<FCustomNavigationConfig>(CurrentNavConfig);

	if (MyCustomConfig.IsValid())
	{
		MyCustomConfig.Get()->SwapAnalogStick(bEnabled);
	}
}


void UGameOptionsFunctionLibrary::SetJoystickDeadZone(const float NewValue)
{
	JoystickDeadZone = NewValue;

	// Navigation
	TSharedPtr<FNavigationConfig> CurrentNavConfig = FSlateApplication::Get().GetNavigationConfig();

	TSharedPtr<FCustomNavigationConfig> MyCustomConfig = StaticCastSharedPtr<FCustomNavigationConfig>(CurrentNavConfig);

	if (MyCustomConfig.IsValid())
	{
		MyCustomConfig.Get()->SetJoystickDeadZone(JoystickDeadZone);
	}
}

// This should not be needed because each setting should have it's default value in config

bool UGameOptionsFunctionLibrary::bCameraShakeEnabled = true;
bool UGameOptionsFunctionLibrary::bGamepadRumbleEnabled = true;
bool UGameOptionsFunctionLibrary::bEnhancedSubtitlesEnabled = false;
bool UGameOptionsFunctionLibrary::bEnhancedHUDEnabled = false;
bool UGameOptionsFunctionLibrary::bManualCameraEnabled = false;
bool UGameOptionsFunctionLibrary::bSwapJoystickEnabled = false;

float UGameOptionsFunctionLibrary::JoystickDeadZone = 0.2f;
float UGameOptionsFunctionLibrary::CameraSensitivity = 0.0f;
float UGameOptionsFunctionLibrary::AimSensitivity = 0.5f;
float UGameOptionsFunctionLibrary::GamepadRumbleIntensity = 1.0f;

bool UGameOptionsFunctionLibrary::bIsFirstLaunch = false;

#pragma endregion Value Getters and Setters

#pragma region Functionalities

void UGameOptionsFunctionLibrary::LoadAllGameOptions(UObject* WorldContextObject)
{
	// for each category
	for (uint8 i = 1; i < static_cast<uint8>(EGameOptionsCategory::MAX); i++)
	{
		EGameOptionsCategory Category = static_cast<EGameOptionsCategory>(i);

		//for each game options available in this platform
		TArray<FInstancedStruct> CategoryGameOptions;
		GetGameOptionsByCategoryWithSaves(WorldContextObject, Category, CategoryGameOptions);
		for (FInstancedStruct& GameOptions : CategoryGameOptions)
		{
			ApplySingleGameOptions(WorldContextObject, GameOptions);
		}
	}
}

void UGameOptionsFunctionLibrary::ChangeGameOptions(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FGameOptionsBaseData* BaseGameOptionsData = GameOptionsData.GetMutablePtr<FGameOptionsBaseData>();

	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(WorldContextObject);

	PersistentUser->GetGameOptions().Add(BaseGameOptionsData->GetId(), BaseGameOptionsData->GetCurrentValueIndex());

	ApplySingleGameOptions(WorldContextObject, GameOptionsData);

	if (UPDWEventSubsytem* EventSubsystem = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnGameOptionsChangedEvent(BaseGameOptionsData->GetId(), BaseGameOptionsData->GetCurrentValueIndex());
	}
}

#pragma endregion Functionalities

#pragma region OverrideFromSystem

void UGameOptionsFunctionLibrary::OverrideSetupGameOptionsData(UObject* WorldContextObject, FInstancedStruct& StructToOverride)
{
	FGameOptionsBaseData* BaseGameOptionsData = StructToOverride.GetMutablePtr<FGameOptionsBaseData>();
	ensure(BaseGameOptionsData);

	switch (BaseGameOptionsData->GetId())
	{
	case EGameOptionsId::Resolution:
		OverrideResolutionSetupGameOptionsData(StructToOverride);
		break;
	case EGameOptionsId::Language:
		OverrideLanguageSetupGameOptionsData(WorldContextObject, StructToOverride);
		break;
	}
}

void UGameOptionsFunctionLibrary::OverrideResolutionSetupGameOptionsData(FInstancedStruct& StructToOverride)
{
	FTextGameOptionsData* TextGameOptionsData = StructToOverride.GetMutablePtr<FTextGameOptionsData>();
	ensure(TextGameOptionsData);

	TArray<FIntPoint> Resolutions;
	UNebulaGraphicsVideoSettingsFunctionLibrary::GetSupportedScreenResolutions(640, 480, Resolutions);
	TArray<FText> ResolutionTexts = TArray<FText>();

	TextGameOptionsData->Values.Empty();
	for (FIntPoint Resolution : Resolutions)
	{
		FString StrRes = FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);
		TextGameOptionsData->Values.Add(FName(*StrRes), FText::FromString(StrRes));
	}

	FIntPoint DesktopResolution;
	UNebulaGraphicsVideoSettingsFunctionLibrary::GetDesktopResolution(DesktopResolution);
	FString Str = FString::FromInt(DesktopResolution.X) + "x" + FString::FromInt(DesktopResolution.Y);
	TextGameOptionsData->DefaultValue = FName(*Str);
	TextGameOptionsData->SetCurrentValueIndex(Resolutions.Num() - 1);
}

void UGameOptionsFunctionLibrary::OverrideLanguageSetupGameOptionsData(UObject* WorldContextObject, FInstancedStruct& StructToOverride)
{
	FTextGameOptionsData* TextGameOptionsData = StructToOverride.GetMutablePtr<FTextGameOptionsData>();
	ensure(TextGameOptionsData);


	TArray<FString> AcceptedCultures = UNebulaFlowLocaleFunctionLibrary::GetCultures(TEXT("AcceptedCultures"));
	FString LocaleToUse = UKismetSystemLibrary::GetDefaultLocale();
	LocaleToUse = UNebulaFlowLocaleFunctionLibrary::AdjustComplexLocale(LocaleToUse, AcceptedCultures);

	TextGameOptionsData->DefaultValue = FName(*LocaleToUse);
	UPDWPersistentUser* CurrentUser = UPDWPersistentUser::GetCurrentOwnerUser(WorldContextObject);
	if (CurrentUser->GetFirstBoot(WorldContextObject))
	{
		TextGameOptionsData->SetToDefault();
	}
	else
	{
		TextGameOptionsData->SetCurrentValueIndex(TextGameOptionsData->GetCurrentValueIndex());
	}
}

#pragma endregion OverrideFromSystem

#pragma region ChangeSettings

void UGameOptionsFunctionLibrary::ApplySingleGameOptions(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FGameOptionsBaseData* BaseGameOptionsData = GameOptionsData.GetMutablePtr<FGameOptionsBaseData>();
	EGameOptionsId Id = BaseGameOptionsData->GetId();

	// #DEV <101-200 range of inputs> [#michele.b, 9 October 2025, ApplySingleGameOptions]
	//if (Id >= static_cast<EGameOptionsId>(101) && Id <= static_cast<EGameOptionsId>(200))
	//{
	//	ChangeRemapInput(WorldContextObject, GameOptionsData);
	//	return;
	//}

	switch (Id)
	{
		// Game
	case EGameOptionsId::Language:
		ChangeLanguage(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Vibration:
		ChangeVibration(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::ManualCamera:
		ChangeManualCamera(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::CameraSensitivity:
		ChangeCameraSensitivity(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::AimSensitivity:
		ChangeAimSensitivity(WorldContextObject, GameOptionsData);
		break;

		// Accessibility
	case EGameOptionsId::ColorDeficiencyType:
	case EGameOptionsId::ColorDeficiencyIntensity:
		ChangeColorDeficiency(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::CameraShake:
		ChangeCameraShake(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::JoyStickDeadZone:
		ChangeJoyStickDeadZone(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::SwapJoystick:
		ChangeSwapJoystick(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::EnhanceSubtitles:
		ChangeEnhanceSubtitles(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::EnhanceHUD:
		ChangeEnhanceHUD(WorldContextObject, GameOptionsData);
		break;

		// Audio
	case EGameOptionsId::MasterVolume:
		ChangeMasterVolume(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::SfxVolume:
		ChangeSfxVolume(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::MusicVolume:
		ChangeMusicVolume(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::DialoguesVolume:
		ChangeDialoguesVolume(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::AudioMono:
		ChangeAudioMono(WorldContextObject, GameOptionsData);
		break;

		// Graphics
	case EGameOptionsId::GeneralGraphicsQuality:
		ChangeGraphicsGeneralQuality(WorldContextObject, GameOptionsData);
		break;
	case EGameOptionsId::WindowMode:
		ChangeWindowMode(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Resolution:
		ChangeResolution(GameOptionsData);
		break;

	case EGameOptionsId::AntiAliasing:
		ChangeAntiAliasing(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Effects:
		ChangeEffects(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Foliage:
		ChangeFoliage(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Gamma:
		ChangeGamma(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::GlobalIllumination:
		ChangeGlobalIllumination(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Landscape:
		ChangeLandscape(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Material:
		ChangeMaterial(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::MaxFPS:
		ChangeMaxFPS(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::PostProcess:
		ChangePostProcess(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Reflection:
		ChangeReflection(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::ResolutionQuality:
		ChangeResolutionQuality(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Shading:
		ChangeShading(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Shadows:
		ChangeShadows(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::Textures:
		ChangeTextures(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::ViewDistance:
		ChangeViewDistance(WorldContextObject, GameOptionsData);
		break;

	case EGameOptionsId::VSync:
		ChangeVSync(WorldContextObject, GameOptionsData);
		break;
	}

	//#DEV <101 - 200 range of Grraphics quality>[#michele.b, 9 October 2025, ApplySingleGameOptions]
	if (Id >= static_cast<EGameOptionsId>(200) && Id <= static_cast<EGameOptionsId>(250))
	{
		CheckChangeGraphicsGeneralQuality(WorldContextObject, GameOptionsData);
	}
}

//Game

void UGameOptionsFunctionLibrary::ChangeLanguage(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FTextGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FTextGameOptionsData>();
	if (!GameOptions) return;

	UNebulaFlowLocaleFunctionLibrary::SetCurrentCulture(GameOptions->GetCurrentKey().ToString());
}

void UGameOptionsFunctionLibrary::ChangeVibration(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* BoolValue = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (BoolValue)
	{
		SetGamepadRumbleEnabled(BoolValue->GetCurrentValue());
	}
	else
	{
		FNumericGameOptionsData* FloatValue = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
		if (!FloatValue)
		{
			return;
		}
		SetGamepadRumbleEnabled(FloatValue->GetCurrentValue() != 0.f);
		SetGamepadRumbleIntensity(FloatValue->GetCurrentValue());
	}
}

void UGameOptionsFunctionLibrary::ChangeManualCamera(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* Value = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!Value) return;

	SetManualCameraEnabled(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeCameraSensitivity(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	SetCameraSensitivity(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeAimSensitivity(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	SetAimSensitivity(Value->GetCurrentValue());
}

// Accessibility

void UGameOptionsFunctionLibrary::ChangeColorDeficiency(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FInstancedStruct Other;
	FGameOptionsBaseData* GameOptions = GameOptionsData.GetMutablePtr<FGameOptionsBaseData>();
	if (!GameOptions) return;

	FNumericGameOptionsData* Value;
	FTextGameOptionsData* Type;
	if (GameOptions->GetId() == EGameOptionsId::ColorDeficiencyType)
	{
		Type = GameOptionsData.GetMutablePtr<FTextGameOptionsData>();
		GetGameOptionsByIdWithSaves(WorldContextObject, EGameOptionsId::ColorDeficiencyIntensity, Other);
		Value = Other.GetMutablePtr<FNumericGameOptionsData>();
	}
	else
	{
		GetGameOptionsByIdWithSaves(WorldContextObject, EGameOptionsId::ColorDeficiencyType, Other);
		Type = Other.GetMutablePtr<FTextGameOptionsData>();
		Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	}
	if (!Type || !Value) return;

	EColorVisionDeficiency Deficiency = EColorVisionDeficiency::NormalVision;
	FName DeficiencyTypeName = Type->GetCurrentKey();

	if (DeficiencyTypeName == FName(UEnum::GetValueAsString(EColorVisionDeficiency::Deuteranope)))
	{
		Deficiency = EColorVisionDeficiency::Deuteranope;
	}
	else if (DeficiencyTypeName == FName(UEnum::GetValueAsString(EColorVisionDeficiency::Protanope)))
	{
		Deficiency = EColorVisionDeficiency::Protanope;
	}
	else if (DeficiencyTypeName == FName(UEnum::GetValueAsString(EColorVisionDeficiency::Tritanope)))
	{
		Deficiency = EColorVisionDeficiency::Tritanope;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *FString::Printf(TEXT("set deficiency to %s at value %.2f"), *UEnum::GetValueAsString(Deficiency), Value->GetCurrentValue()));

	UWidgetBlueprintLibrary::SetColorVisionDeficiencyType(Deficiency, Value->GetCurrentValue(), true, false);
}

void UGameOptionsFunctionLibrary::ChangeCameraShake(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* Value = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!Value) return;

	SetCameraShakeEnabled(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeJoyStickDeadZone(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	SetJoystickDeadZone(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeSwapJoystick(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* Value = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!Value) return;

	SetSwapJoystickEnabled(WorldContextObject, Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeEnhanceSubtitles(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* Value = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!Value) return;

	SetEnhancedSubtitlesEnabled(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeEnhanceHUD(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* Value = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!Value) return;

	SetEnhancedHUDEnabled(Value->GetCurrentValue());
}

// Audio

void UGameOptionsFunctionLibrary::ChangeMasterVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	UPDWAudioManager::Get(WorldContextObject)->SetVolumeMaster(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeSfxVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	UPDWAudioManager::Get(WorldContextObject)->SetVolumeSfx(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeMusicVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	UPDWAudioManager::Get(WorldContextObject)->SetMusicVolume(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeDialoguesVolume(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* Value = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!Value) return;

	UPDWAudioManager::Get(WorldContextObject)->SetVolumeVoices(Value->GetCurrentValue());
}

void UGameOptionsFunctionLibrary::ChangeAudioMono(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* Value = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!Value) return;

	UPDWAudioManager::Get(WorldContextObject)->ToggleMonoSnapShot(Value->GetCurrentValue());
}

// Controls and Custom

void UGameOptionsFunctionLibrary::ChangeRemapInput(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	// #TODO_PDW <> [#michele.b, 9 October 2025, ChangeRemapInput]
}

// Graphics

void UGameOptionsFunctionLibrary::CheckChangeGraphicsGeneralQuality(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FGameOptionsBaseData* BaseGameOptionsData = GameOptionsData.GetMutablePtr<FGameOptionsBaseData>();	
	FInstancedStruct GeneralGraphicsGameOptions;			
	GetGameOptionsByIdWithSaves(WorldContextObject, EGameOptionsId::GeneralGraphicsQuality, GeneralGraphicsGameOptions);
	FAggregatedQualityGameOptionsData* GeneralQualityGameOptions = GeneralGraphicsGameOptions.GetMutablePtr<FAggregatedQualityGameOptionsData>();
	if (GeneralQualityGameOptions && GeneralQualityGameOptions->AffectedQualities.Contains(BaseGameOptionsData->GetId()))
	{
		if (BaseGameOptionsData->GetCurrentValueIndex() != GeneralQualityGameOptions->GetCurrentValueIndex() && GeneralQualityGameOptions->GetCurrentValue() != EQualityGameOptionsValue::Custom)
		{
			GeneralQualityGameOptions->SetCurrentValue(EQualityGameOptionsValue::Custom);
			ChangeGameOptions(WorldContextObject, GeneralGraphicsGameOptions);
		}
	}
}

void UGameOptionsFunctionLibrary::ChangeGraphicsGeneralQuality(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FAggregatedQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FAggregatedQualityGameOptionsData>();
	if (!GameOptions) return;
	EQualityGameOptionsValue QualityValue = GameOptions->GetCurrentValue();
	int32 Value = GameOptions->GetCurrentValueIndex();
	if (QualityValue != EQualityGameOptionsValue::Custom)
	{
		for (EGameOptionsId Affected : GameOptions->AffectedQualities)
		{
			FInstancedStruct AffectedGameOptions;			
			GetGameOptionsByIdWithSaves(WorldContextObject, Affected, AffectedGameOptions);
			FQualityGameOptionsData* QualityGameOptions = AffectedGameOptions.GetMutablePtr<FQualityGameOptionsData>();			
			if(QualityGameOptions)
			{
				QualityGameOptions->SetCurrentValue(QualityValue);
				ChangeGameOptions(WorldContextObject, AffectedGameOptions);
			}
			else
			{
				FTextGameOptionsData* TextGameOptions = AffectedGameOptions.GetMutablePtr<FTextGameOptionsData>();
				if (TextGameOptions)
				{
					TextGameOptions->SetCurrentValueIndex(FMath::FloorLog2((int32)QualityValue));
					ChangeGameOptions(WorldContextObject, AffectedGameOptions);
				}
			}
			
		}
	}

}

void UGameOptionsFunctionLibrary::ChangeWindowMode(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FTextGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FTextGameOptionsData>();
	if (!GameOptions) return;

	FString bo = UEnum::GetValueAsString(EWindowMode::Fullscreen);
	if (GameOptions->GetCurrentKey() == FName(UEnum::GetValueAsString(EWindowMode::Fullscreen)))
	{
		UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyWindowMode(EWindowMode::Fullscreen);
	}
	else if (GameOptions->GetCurrentKey() == FName(UEnum::GetValueAsString(EWindowMode::Windowed)))
	{
		UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyWindowMode(EWindowMode::Windowed);
	}
	else if (GameOptions->GetCurrentKey() == FName(UEnum::GetValueAsString(EWindowMode::WindowedFullscreen)))
	{
		UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyWindowMode(EWindowMode::WindowedFullscreen);
	}
}

void UGameOptionsFunctionLibrary::ChangeResolution(FInstancedStruct& GameOptionsData)
{
	FTextGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FTextGameOptionsData>();
	if (!GameOptions) return;

	int32 Width = 0;
	int32 Height = 0;
	TArray<FString> Parts;

	// Split into two parts by "x"
	if (GameOptions->GetCurrentValue().ToString().Split(TEXT("x"), &Parts.AddDefaulted_GetRef(), &Parts.AddDefaulted_GetRef()))
	{
		Width = FCString::Atoi(*Parts[1]);
		Height = FCString::Atoi(*Parts[0]);
	}

	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyScreenResolution(Width, Height);
}

void UGameOptionsFunctionLibrary::ChangeAntiAliasing(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FTextGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FTextGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyAntiAliasingQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeEffects(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyEffectsQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeFoliage(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyFoliageQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeGamma(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FNumericGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FNumericGameOptionsData>();
	if (!GameOptions) return;

	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyGamma(WorldContextObject, GameOptions->GetCurrentValue() * 2.2f / 6);
}

void UGameOptionsFunctionLibrary::ChangeGlobalIllumination(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyGlobalIlluminationQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeLandscape(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyLandscapeQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeMaterial(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyMaterialQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeMaxFPS(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FTextGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FTextGameOptionsData>();
	if (!GameOptions) return;

	FString StringValue = GameOptions->GetCurrentKey().ToString().TrimStartAndEnd();

	float FPS = 0;
	if (StringValue.IsNumeric())
	{
		FPS = FCString::Atoi(*StringValue);
	}

	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyFrameRateLimit(FPS);
}

void UGameOptionsFunctionLibrary::ChangePostProcess(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyPostProcessQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeReflection(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyReflectionQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeResolutionQuality(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyResolutionQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeShading(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyShadingQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeShadows(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyShadowQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeTextures(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyTextureQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeViewDistance(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FQualityGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FQualityGameOptionsData>();
	if (!GameOptions) return;

	int32 Value = GameOptions->GetCurrentValueIndex();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyViewDistanceQuality(WorldContextObject, Value);
}

void UGameOptionsFunctionLibrary::ChangeVSync(UObject* WorldContextObject, FInstancedStruct& GameOptionsData)
{
	FBoolGameOptionsData* GameOptions = GameOptionsData.GetMutablePtr<FBoolGameOptionsData>();
	if (!GameOptions) return;

	bool Value = GameOptions->GetCurrentValue();
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyVSyncEnabled(Value);
}

#pragma endregion ChangeSettings
