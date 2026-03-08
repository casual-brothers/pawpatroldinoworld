// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ToxicUtilitiesSetting.generated.h"

/**
 * 
 */
UCLASS(Config = EditorPerProjectUserSettings, meta = (DisplayName = "Toxic Utilities"))
class TOXICUTILITIES_API UToxicUtilitiesSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	

	public:
	UToxicUtilitiesSetting(const FObjectInitializer& ObjectInitializer);

	static UToxicUtilitiesSetting* Get() { return StaticClass()->GetDefaultObject<UToxicUtilitiesSetting>(); }
	
	void SetSaveSystemDisabled(const bool inValue);
	void SetAutoSkipCutscene(const bool inValue);
	void SetAudioDisabled(const bool inValue);
	void SetTutorialDisabled(const bool inValue);

	UPROPERTY(config,EditAnywhere,Meta=(ConfigRestartRequired=false))
	bool bDisableSavingSystem = false;

	UPROPERTY(config,EditAnywhere,Meta=(ConfigRestartRequired=false))
	bool bAutoSkipAllCutscenes = false;

	UPROPERTY(config,EditAnywhere,Meta=(ConfigRestartRequired=false))
	bool bDisableAudio = false;

	UPROPERTY(config, EditAnywhere, Meta = (ConfigRestartRequired = false))
	bool bDisableTutorial = false;

	virtual FName GetCategoryName() const override { return FName("Toxic Utilities"); }
	virtual FText GetSectionText() const override { return INVTEXT("User Settings"); }
};
