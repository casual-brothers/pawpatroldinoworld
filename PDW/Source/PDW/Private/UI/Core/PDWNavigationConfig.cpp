// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/PDWNavigationConfig.h"
#include "GameFramework/InputSettings.h" // from Engine
 
// based on the engine's FNavigationConfig code:
// Engine/Source/Runtime/Slate/Public/Framework/Application/NavigationConfig.cpp
 
FCustomNavigationConfig::FCustomNavigationConfig()
{
	KeyEventRules.Reset();
	bTabNavigation = false;
	bKeyNavigation = true;
	bAnalogNavigation = true;

	AnalogHorizontalKey = EKeys::Gamepad_LeftX;
	AnalogVerticalKey = EKeys::Gamepad_LeftY;

	KeyEventRules.Emplace(EKeys::Left, EUINavigation::Left);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Left, EUINavigation::Left);

	KeyEventRules.Emplace(EKeys::Right, EUINavigation::Right);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Right, EUINavigation::Right);

	KeyEventRules.Emplace(EKeys::Up, EUINavigation::Up);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Up, EUINavigation::Up);

	KeyEventRules.Emplace(EKeys::Down, EUINavigation::Down);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Down, EUINavigation::Down);

	// By default, enter, space, and gamepad accept are all counted as accept
	KeyActionRules.Emplace(EKeys::Enter, EUINavigationAction::Accept);
	KeyActionRules.Emplace(EKeys::SpaceBar, EUINavigationAction::Accept);
	KeyActionRules.Emplace(EKeys::Virtual_Accept, EUINavigationAction::Accept);

	// By default, escape and gamepad back count as leaving current scope
	KeyActionRules.Emplace(EKeys::Escape, EUINavigationAction::Back);
	KeyActionRules.Emplace(EKeys::Virtual_Back, EUINavigationAction::Back);
}

void FCustomNavigationConfig::SwapAnalogStick(bool bUseRightInsteadOfLeft)
{
	if (bUseRightInsteadOfLeft)
	{
		AnalogHorizontalKey = EKeys::Gamepad_RightX;
		AnalogVerticalKey = EKeys::Gamepad_RightY;
	}
	else
	{
		AnalogHorizontalKey = EKeys::Gamepad_LeftX;
		AnalogVerticalKey = EKeys::Gamepad_LeftY;
	}
}

void FCustomNavigationConfig::SetJoystickDeadZone(float NewDeadZone)
{
	AnalogNavigationHorizontalThreshold = NewDeadZone;
	AnalogNavigationVerticalThreshold = NewDeadZone;
}

float FCustomNavigationConfig::GetRepeatRateForPressure(float InPressure, int32 InRepeats) const
{
	// slower than default 0.5f 
	return 0.7f;
}
