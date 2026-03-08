// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Modifiers/CustomDeadZoneInputModifier.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"

FInputActionValue UCustomDeadZoneInputModifier::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	float LowerThreshold = UGameOptionsFunctionLibrary::GetJoystickDeadZone();
	float UpperThreshold = 1.0f;
	// Can't apply a deadzone to a boolean type (0 or 1 are the only options) 
	EInputActionValueType ValueType = CurrentValue.GetValueType();
	if (ValueType == EInputActionValueType::Boolean)
	{
		return CurrentValue;
	}

	auto DeadZoneLambda = [this, LowerThreshold, UpperThreshold](const float AxisVal) -> float
		{
			// We need to translate and scale the input to the +/- 1 range after removing the dead zone.
			return FMath::Min(1.f, (FMath::Max(0.f, FMath::Abs(AxisVal) - LowerThreshold) / (UpperThreshold - LowerThreshold))) * FMath::Sign(AxisVal);
		};

	auto UnscaledDeadZoneLambda = [this, LowerThreshold, UpperThreshold](const float AxisVal)-> float
		{
			// If the value is less then our lower threshold, return zero
			// otherwise, clamp the value to the upper threshold.
			return
				AxisVal < LowerThreshold ?
				0.0f :
				FMath::Min(AxisVal, UpperThreshold);
		};

	FVector NewValue = CurrentValue.Get<FVector>();
	switch (Type)
	{
	case EDeadZoneType::Axial:
		NewValue.X = DeadZoneLambda(NewValue.X);
		NewValue.Y = DeadZoneLambda(NewValue.Y);
		NewValue.Z = DeadZoneLambda(NewValue.Z);
		break;
	case EDeadZoneType::Radial:
		if (ValueType == EInputActionValueType::Axis3D)
		{
			NewValue = NewValue.GetSafeNormal() * DeadZoneLambda(NewValue.Size());
		}
		else if (ValueType == EInputActionValueType::Axis2D)
		{
			NewValue = NewValue.GetSafeNormal2D() * DeadZoneLambda(NewValue.Size2D());
		}
		else
		{
			NewValue.X = DeadZoneLambda(NewValue.X);
		}
		break;
	case EDeadZoneType::UnscaledRadial:
		if (ValueType == EInputActionValueType::Axis3D)
		{
			NewValue = NewValue.GetSafeNormal() * UnscaledDeadZoneLambda(NewValue.Size());
		}
		else if (ValueType == EInputActionValueType::Axis2D)
		{
			NewValue = NewValue.GetSafeNormal2D() * UnscaledDeadZoneLambda(NewValue.Size2D());
		}
		else
		{
			NewValue.X = UnscaledDeadZoneLambda(NewValue.X);
		}
		break;
	}

	return NewValue;
};

FLinearColor UCustomDeadZoneInputModifier::GetVisualizationColor_Implementation(FInputActionValue SampleValue, FInputActionValue FinalValue) const
{
	// Visualize as black when unmodified. Red when blocked (with differing intensities to indicate axes)
	// Mirrors visualization in https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php.
	if (FinalValue.GetValueType() == EInputActionValueType::Boolean || FinalValue.GetValueType() == EInputActionValueType::Axis1D)
	{
		return FLinearColor(FinalValue.Get<float>() == 0.f ? 1.f : 0.f, 0.f, 0.f);
	}
	return FLinearColor((FinalValue.Get<FVector2D>().X == 0.f ? 0.5f : 0.f) + (FinalValue.Get<FVector2D>().Y == 0.f ? 0.5f : 0.f), 0.f, 0.f);
}
