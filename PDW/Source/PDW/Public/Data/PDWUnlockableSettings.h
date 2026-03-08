// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PDWGameplayStructures.h"
#include "GameplayTagContainer.h"
#include "PDWUnlockableSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "UnlockableSettings"))
class PDW_API UPDWUnlockableSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
	
	static UPDWUnlockableSettings* Get();

public:

	UFUNCTION()
	UPDWLevelUpRewardsData* GetLevelUpSettings();

protected:

	UPROPERTY(Config, EditAnywhere)
	FSoftObjectPath LevelUpConfig;
	
};
