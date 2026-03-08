// Fill out your copyright notice in the Description page of Project Settings.


#include "ToxicUtilitiesSetting.h"

UToxicUtilitiesSetting::UToxicUtilitiesSetting(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
, bDisableSavingSystem(false)
,bAutoSkipAllCutscenes(false)
,bDisableAudio(false)
,bDisableTutorial(false)
{

}

void UToxicUtilitiesSetting::SetSaveSystemDisabled(const bool inValue)
{
	bDisableSavingSystem = inValue;
	SaveConfig();
}

void UToxicUtilitiesSetting::SetAutoSkipCutscene(const bool inValue)
{
	bAutoSkipAllCutscenes = inValue;
	SaveConfig();
}

void UToxicUtilitiesSetting::SetAudioDisabled(const bool inValue)
{
	bDisableAudio = inValue;
	SaveConfig();
}

void UToxicUtilitiesSetting::SetTutorialDisabled(const bool inValue)
{
	bDisableTutorial = inValue;
	SaveConfig();
}