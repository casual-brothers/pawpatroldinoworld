// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Modifiers/CustomInputModifierNegate.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "InputActionValue.h"
#include "EnhancedPlayerInput.h"


FInputActionValue UCustomInputModifierNegate::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (UGameOptionsFunctionLibrary::GetSwapJoystickEnabled())
	{
	return CurrentValue.Get<FVector>() * FVector(bX ? -1.f : 1.f, bY ? -1.f : 1.f, bZ ? -1.f : 1.f);
	}
	else
	{
		return CurrentValue;
	}
}

FLinearColor UCustomInputModifierNegate::GetVisualizationColor_Implementation(FInputActionValue SampleValue, FInputActionValue FinalValue) const
{
	FVector Sample = SampleValue.Get<FVector>();
	FVector Final = FinalValue.Get<FVector>();
	return FLinearColor(Sample.X != Final.X ? 1.f : 0.f, Sample.Y != Final.Y ? 1.f : 0.f, Sample.Z != Final.Z ? 1.f : 0.f);
}