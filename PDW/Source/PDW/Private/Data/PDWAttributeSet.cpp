// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWAttributeSet.h"
#include "GameplayEffectExtension.h"


void UPDWAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetCharacterSpeedAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxCharacterSpeed());
		return;
	}
	else if (Attribute == GetCharacterTurningAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxCharacterTurning());
		return;
	}
	else if (Attribute == GetCharacterAccelerationAttribute())
	{
		float temp = GetCharacterAcceleration();
		NewValue = FMath::Clamp<float>((int)NewValue, 0.0f, GetMaxCharacterAcceleration());
		return;
	}
	else if (Attribute == GetVehicleSpeedAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxVehicleSpeed());
		return;
	}
	else if (Attribute == GetVehicleTurningAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxVehicleTurning());
		return;
	}
	else if (Attribute == GetVehicleAccelerationAttribute())
	{
		float temp = GetVehicleAcceleration();
		NewValue = FMath::Clamp<float>((int)NewValue, 0.0f, GetMaxVehicleAcceleration());
		return;
	}
}

void UPDWAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCharacterAccelerationAttribute())
	{
		SetCharacterAcceleration(FMath::Clamp(GetCharacterAcceleration(), 0.0f, GetMaxCharacterAcceleration()));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetCharacterSpeedAttribute())
	{
		SetCharacterSpeed(FMath::Clamp(GetCharacterSpeed(), 0.0f, GetMaxCharacterSpeed()));
		return;
	}
}
