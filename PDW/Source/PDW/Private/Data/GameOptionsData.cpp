// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Data/GameOptionsData.h"
#include "Data/GameOptionsDeveloperSettings.h"

#pragma region GameOptionsBaseData

float FGameOptionsBaseData::GetCurrentPercent() const
{
	if (GetNumberOfValues() <= 1) return 0.f;
	return ((float)CurrentValueIndex / (GetNumberOfValues() - 1));
}

void FGameOptionsBaseData::SetCurrentValueIndex(int32 InValueIndex)
{
	if (GetNumberOfValues() < 1) return;
	while (InValueIndex < 0)
	{
		InValueIndex += GetNumberOfValues();
	}
	CurrentValueIndex = (InValueIndex + GetNumberOfValues()) % GetNumberOfValues();
}

bool FGameOptionsBaseData::GetIsAvailableInCurrentPlatform() const
{
#if PLATFORM_PS4
	return (PlatformAvailability & static_cast<int32>(EPlatformAvailability::PS4)) != 0;
#elif PLATFORM_PS5
	return (PlatformAvailability & static_cast<int32>(EPlatformAvailability::PS5)) != 0;
#elif PLATFORM_XBOXONE
	return (PlatformAvailability & static_cast<int32>(EPlatformAvailability::XB1)) != 0;
#elif PLATFORM_XSX
	return (PlatformAvailability & static_cast<int32>(EPlatformAvailability::XSX)) != 0;
#elif PLATFORM_SWITCH
	return (PlatformAvailability & static_cast<int32>(EPlatformAvailability::Switch)) != 0;
#else
	return (PlatformAvailability & static_cast<int32>(EPlatformAvailability::Windows)) != 0;
#endif
}

#pragma endregion GameOptionsBaseData

#pragma region NumericGameOptionsData

int32 FNumericGameOptionsData::GetNumberOfValues() const
{
	return NumberOfValues;
}

FText FNumericGameOptionsData::GetCurrentValueText() const
{
	return FText::AsNumber(GetCurrentValue());
}

void FNumericGameOptionsData::SetToDefault()
{
	SetCurrentValueIndex(DefaultValueIndex);
}

float FNumericGameOptionsData::GetCurrentValue() const
{
	return MinValue + GetCurrentPercent() * (MaxValue - MinValue);
}

float FNumericGameOptionsData::GetDefaultValue() const
{
	return MinValue + ((float)DefaultValueIndex / (GetNumberOfValues() - 1)) * (MaxValue - MinValue);
}

#pragma endregion NumericGameOptionsData

#pragma region BoolGameOptionsData

int32 FBoolGameOptionsData::GetNumberOfValues() const
{
	return 2;
}

FText FBoolGameOptionsData::GetCurrentValueText() const
{	
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (GameOptionsDevSet)
	{
		return GameOptionsDevSet->BoolTexts[GetCurrentValue()];
	}
	else
	{	
		return FText();
	}
}

bool FBoolGameOptionsData::GetCurrentValue() const
{
	return CurrentValueIndex > 0;
}

void FBoolGameOptionsData::SetCurrentValue(bool bValue)
{
	CurrentValueIndex = bValue ? 1 : 0;
}

void FBoolGameOptionsData::SetToDefault()
{
	CurrentValueIndex = DefaultValue ? 1 : 0;
}

#pragma endregion BoolGameOptionsData

#pragma region TextGameOptionsData

int32 FTextGameOptionsData::GetNumberOfValues() const
{
	return Values.Num();
}

FText FTextGameOptionsData::GetCurrentValueText() const
{
	return GetCurrentValue();
}

void FTextGameOptionsData::SetToDefault()
{	
	FText RealDefaultValue = GetDefaultValue();
	TArray<FName> Keys;
	Values.GetKeys(Keys);
	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		if (Values[Keys[i]].EqualTo(RealDefaultValue))
		{
			CurrentValueIndex = i;
			return;
		}
	}
}

FName FTextGameOptionsData::GetCurrentKey() const
{
	TArray<FName> Keys;
	Values.GetKeys(Keys);
	return Keys[CurrentValueIndex];
}

FText FTextGameOptionsData::GetCurrentValue() const
{
	TArray<FName> Keys;
	Values.GetKeys(Keys);
	if (Keys.IsEmpty())
	{
		return FText();
	}
	return Values[Keys[CurrentValueIndex%Keys.Num()]];
}

FText FTextGameOptionsData::GetDefaultValue() const
{
	if (Values.Contains(DefaultValue))
	{
		return Values[DefaultValue];
	}
	else
	{
		TArray<FName> Keys;
		Values.GetKeys(Keys);
		return Keys.Num() > 0 ? Values[Keys[0]] : FText();
	}
}

#pragma endregion TextGameOptionsData

#pragma region QualityGameOptionsData

int32 FQualityGameOptionsData::GetNumberOfValues() const
{
	return FMath::CountBits(QualityAvailability);
}

FText FQualityGameOptionsData::GetCurrentValueText() const
{
	const UGameOptionsDeveloperSettings* GameOptionsDevSet = UGameOptionsDeveloperSettings::Get();
	if (GameOptionsDevSet)
	{
		return GameOptionsDevSet->QualityTexts[GetCurrentValue()];
	}
	else
	{	
		return FText();
	}
}

void FQualityGameOptionsData::SetCurrentValue(EQualityGameOptionsValue Value)
{
	int32 Index = 0;

	int32 ValueInt = static_cast<int32>(Value);

	for (int32 i = 0; i < 32; i++)
	{
		int32 Mask = (1 << i);

		// Only consider bits that are actually available
		if ((QualityAvailability & Mask) != 0)
		{
			if (ValueInt == Mask)
			{
				CurrentValueIndex = Index;
				return;
			}
			Index++;
		}
	}
	// fallback
	CurrentValueIndex = 0;
}

EQualityGameOptionsValue FQualityGameOptionsData::GetCurrentValue() const
{
	int32 Index = 0;
	for (int32 i = 0; i < 32; i++)
	{
		int32 Mask = (1 << i);
		if (QualityAvailability & Mask)
		{
			if (Index == CurrentValueIndex)
			{
				return static_cast<EQualityGameOptionsValue>(Mask);
			}
			Index++;
		}
	}

	// Fallback: return the first available bit
	for (int32 i = 0; i < 32; i++)
	{
		int32 Mask = (1 << i);
		if (QualityAvailability & Mask)
		{
			return static_cast<EQualityGameOptionsValue>(Mask);
		}
	}

	// If nothing is available at all, return medium
	return EQualityGameOptionsValue::Medium;
}

void FQualityGameOptionsData::SetToDefault()
{
	SetCurrentValue(DefaultValue);
	DefaultValue = GetCurrentValue();
}

#pragma endregion QualityGameOptionsData
