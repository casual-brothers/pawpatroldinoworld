// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Widgets/Input/SCheckBox.h"

class FToolBarBuilder;
class FMenuBuilder;

static const FName FirstNomadTab = TEXT("Toxic Sun Utilities");

UENUM()
enum class EBoolOptions : uint8
{
	CutsceneSkip,
	AutoSave,
	Audio,
	Tutorial
};

class FToxicUtilitiesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
	TSharedRef<SDockTab> SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs);

	FReply DeleteSaves();
	ECheckBoxState DisableSaveSystem(EBoolOptions inOption) const;
	void OnDisableSaveSystem(ECheckBoxState NewCHeckedState,EBoolOptions inOption);
private:

	void RegisterMenus();


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
