// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "GameOptionsEnums.generated.h"

// Sum = 1+ 2 + 4 + 8 + 16 + 32 = 63  || 2^(6) -1 = 63
UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlatformAvailability : uint8
{
	Windows = 1 << 0,
	PS4 = 1 << 1,
	PS5 = 1 << 2,
	XB1 = 1 << 3,
	XSX = 1 << 4,
	Switch = 1 << 5,

	MAX
};

UENUM(BlueprintType)
enum class EGameOptionsCategory : uint8
{
	E_NONE = 0		UMETA(DisplayName = "NONE"),

	Game,
	Audio,
	Controls,
	Graphics,
	Eula,
	Accessibility,

	MAX
};


UENUM(BlueprintType)
enum class EGameOptionsId : uint8
{
	E_None = 0	UMETA(Hidden),

	//Game 1-30
	Language = 1,
	Vibration = 2,
	ManualCamera = 3,
	CameraSensitivity = 4,
	AimSensitivity = 5,

	//Accessibility 31-50
	ColorDeficiencyType = 31,
	ColorDeficiencyIntensity = 32,
	CameraShake = 33,
	JoyStickDeadZone = 34,
	SwapJoystick = 35,
	EnhanceSubtitles = 36,
	EnhanceHUD = 37,

	//Audio 51-100
	MasterVolume = 51,
	SfxVolume = 52,
	MusicVolume = 53,
	DialoguesVolume = 54,
	AudioMono = 55,

	// Custom 101-199

	//Graphics 200-250
	GeneralGraphicsQuality = 200,
	WindowMode = 201,
	Resolution = 202,
	AntiAliasing = 203,
	Blur = 204,
	Effects = 205,
	Foliage = 206,
	Gamma = 207,
	GlobalIllumination = 208,
	Landscape = 209,
	Material = 210,
	MaxFPS = 211,
	PostProcess = 212,
	Reflection = 213,
	ResolutionQuality = 214,
	Shading = 215,
	Shadows = 216,
	Textures = 217,
	ViewDistance = 218,
	VSync = 219,

	//Eula
	Eula = 252
};


// Sum = 1 + 2 + 4 + 8  = 15  || 2^(4) -1 = 15
UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EQualityGameOptionsValue : uint8
{
	Low = 1 << 0,
	Medium = 1 << 1,
	High = 1 << 2,
	Epic = 1 << 3,
	Custom = 1 << 4,

	MAX
};

