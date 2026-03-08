// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Data/GameOptionsDeveloperSettings.h"



const UGameOptionsDeveloperSettings* UGameOptionsDeveloperSettings::Get()
{
	const UGameOptionsDeveloperSettings* Settings = GetDefault<UGameOptionsDeveloperSettings>();
	ensureMsgf(Settings, TEXT("Unable to get Nebula Flow Game Settings!"));
	return Settings;
}


#if WITH_EDITOR
void UGameOptionsDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	for (auto& Pair : GameOptionsData)
	{
		for (FInstancedStruct& InstStruct : Pair.Value.GameOptions)
		{
			if (!InstStruct.IsValid())
				continue;

			const UScriptStruct* ScriptStruct = InstStruct.GetScriptStruct();
			if (ScriptStruct && ScriptStruct->IsChildOf(FGameOptionsBaseData::StaticStruct()))
			{
				FGameOptionsBaseData* SingleGameOptionsData = InstStruct.GetMutablePtr<FGameOptionsBaseData>();
				if (SingleGameOptionsData)
				{
					SingleGameOptionsData->SetToDefault();
				}
				FNumericGameOptionsData* NumericGameOptionsData = InstStruct.GetMutablePtr<FNumericGameOptionsData>();
				if (NumericGameOptionsData)
				{
					NumericGameOptionsData->DefaultValueIndex = FMath::Min(NumericGameOptionsData->DefaultValueIndex, NumericGameOptionsData->GetNumberOfValues());
					NumericGameOptionsData->SetToDefault();
				}
			}
		}
	}
	UGameOptionsDeveloperSettings* Settings = GetMutableDefault<UGameOptionsDeveloperSettings>();
	Settings->SaveConfig();
	Settings->TryUpdateDefaultConfigFile();
}

TArray<FName> UGameOptionsDeveloperSettings::GetDefaultSelectorsKeyOptions() const
{
	TArray<FName> Options;

	for (TObjectIterator<UScriptStruct> It; It; ++It)
	{
		UScriptStruct* Struct = *It;
		if (Struct->IsChildOf(FGameOptionsBaseData::StaticStruct()) && Struct != FGameOptionsBaseData::StaticStruct())
		{
			Options.Add(Struct->GetFName());
		}
	}

	return Options;
}

#endif
