// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWUnlockableSettings.h"
#include "Data/PDWGameplayStructures.h"

UPDWUnlockableSettings::UPDWUnlockableSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPDWUnlockableSettings* UPDWUnlockableSettings::Get()
{
	return CastChecked<UPDWUnlockableSettings>(UPDWUnlockableSettings::StaticClass()->GetDefaultObject());
}

UPDWLevelUpRewardsData* UPDWUnlockableSettings::GetLevelUpSettings()
{
	UPDWLevelUpRewardsData* Config = Cast<UPDWLevelUpRewardsData>(LevelUpConfig.TryLoad());
	return Config;
}
