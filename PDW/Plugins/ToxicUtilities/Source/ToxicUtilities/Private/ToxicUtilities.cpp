// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToxicUtilities.h"
#include "ToxicUtilitiesStyle.h"
#include "ToxicUtilitiesCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelEditor.h"
#include "Styling/SlateTypes.h"
#include "ToxicUtilitiesSetting.h"

static const FName ToxicUtilitiesTabName("ToxicUtilities");

#define LOCTEXT_NAMESPACE "FToxicUtilitiesModule"

void FToxicUtilitiesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FToxicUtilitiesStyle::Initialize();
	FToxicUtilitiesStyle::ReloadTextures();

	FToxicUtilitiesCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FToxicUtilitiesCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FToxicUtilitiesModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FToxicUtilitiesModule::RegisterMenus));
}

void FToxicUtilitiesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FToxicUtilitiesStyle::Shutdown();

	FToxicUtilitiesCommands::Unregister();
}

void FToxicUtilitiesModule::PluginButtonClicked()
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	Extender->AddToolBarExtension("Settings", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder& Builder)
		{
			Builder.AddToolBarButton(FUIAction(), NAME_None, FText::FromString("HEY!"));
		}));
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(Extender);

	const TSharedRef<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	FTabSpawnerEntry SpawnEntry = TabManager->RegisterNomadTabSpawner(FirstNomadTab, FOnSpawnTab::CreateRaw(this, &FToxicUtilitiesModule::SpawnNomadTab));
	TabManager->TryInvokeTab(FirstNomadTab);
}

TSharedRef<SDockTab> FToxicUtilitiesModule::SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(8, 8, 0, 0).HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SButton)
			.Text(LOCTEXT("Order66", "Panata"))
		.OnClicked_Raw(this, &FToxicUtilitiesModule::DeleteSaves)
		.ToolTipText(LOCTEXT("SaveButton", "Delete Saves"))
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(8, 8, 0, 0).HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged_Raw(this,&FToxicUtilitiesModule::OnDisableSaveSystem,EBoolOptions::AutoSave)
			.IsChecked_Raw(this,&FToxicUtilitiesModule::DisableSaveSystem,EBoolOptions::AutoSave)
			.ToolTipText(LOCTEXT("OptionsTip", "Disable Save System"))
			.Content()
				[
					SNew(STextBlock)
						.Text(LOCTEXT("ToggleSaveSystem", "Disable Save System"))
						.Font(FAppStyle::GetFontStyle("StandardDialog.SmallFont"))
				]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(8, 8, 0, 0).HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged_Raw(this,&FToxicUtilitiesModule::OnDisableSaveSystem,EBoolOptions::CutsceneSkip)
			.IsChecked_Raw(this,&FToxicUtilitiesModule::DisableSaveSystem,EBoolOptions::CutsceneSkip)
			.ToolTipText(LOCTEXT("Cutscene", "Enable AutoSkip Cutscene"))
			.Content()
				[
					SNew(STextBlock)
						.Text(LOCTEXT("ToggleCutsceneSkip", "Enable AutoSkip Cutscene"))
						.Font(FAppStyle::GetFontStyle("StandardDialog.SmallFont"))
				]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(8, 8, 0, 0).HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged_Raw(this,&FToxicUtilitiesModule::OnDisableSaveSystem,EBoolOptions::Audio)
			.IsChecked_Raw(this,&FToxicUtilitiesModule::DisableSaveSystem,EBoolOptions::Audio)
			.ToolTipText(LOCTEXT("Audio", "Disable Audio"))
			.Content()
				[
					SNew(STextBlock)
						.Text(LOCTEXT("ToggleAudio", "Disable Audio Soundtrack"))
						.Font(FAppStyle::GetFontStyle("StandardDialog.SmallFont"))
				]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(8, 8, 0, 0).HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged_Raw(this,&FToxicUtilitiesModule::OnDisableSaveSystem,EBoolOptions::Tutorial)
			.IsChecked_Raw(this,&FToxicUtilitiesModule::DisableSaveSystem,EBoolOptions::Tutorial)
			.ToolTipText(LOCTEXT("Tutorial", "Disable Tutorial"))
			.Content()
				[
					SNew(STextBlock)
						.Text(LOCTEXT("ToggleTutorial", "Disable Tutorial"))
						.Font(FAppStyle::GetFontStyle("StandardDialog.SmallFont"))
				]
		]
		];
}

FReply FToxicUtilitiesModule::DeleteSaves()
{
	FString Path = UKismetSystemLibrary::GetProjectSavedDirectory() / TEXT("SaveGames");
	if(!Path.IsEmpty())
	{ 
		IFileManager& FileManager = IFileManager::Get();
		TArray<FString> Files;
		FileManager.FindFiles(Files,*Path);
		for (FString File : Files)
		{
			FString CompletePath = Path/File;
			if (FPaths::ValidatePath(CompletePath) && FPaths::FileExists(CompletePath))
			{
				FileManager.Delete(*CompletePath);
			}
		}
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Message", "De' boia"));
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

ECheckBoxState FToxicUtilitiesModule::DisableSaveSystem(EBoolOptions inOption) const 
{
	UToxicUtilitiesSetting* Settings = UToxicUtilitiesSetting::Get();
	ECheckBoxState Value;
	if (inOption == EBoolOptions::AutoSave)
	{
		Value = Settings->bDisableSavingSystem ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	else if (inOption == EBoolOptions::CutsceneSkip)
	{
		Value = Settings->bAutoSkipAllCutscenes ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	else if (inOption == EBoolOptions::Audio)
	{
		Value = Settings->bDisableAudio ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	else if (inOption == EBoolOptions::Tutorial)
	{
		Value = Settings->bDisableTutorial ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return Value; 
}

void FToxicUtilitiesModule::OnDisableSaveSystem(ECheckBoxState NewCHeckedState,EBoolOptions inOption)
{
	UToxicUtilitiesSetting* Settings = UToxicUtilitiesSetting::Get();
	if (inOption == EBoolOptions::AutoSave)
	{
		Settings->SetSaveSystemDisabled(NewCHeckedState == ECheckBoxState::Checked);
	}
	else if (inOption == EBoolOptions::CutsceneSkip)
	{
		Settings->SetAutoSkipCutscene(NewCHeckedState == ECheckBoxState::Checked);
	}
	else if (inOption == EBoolOptions::Audio)
	{
		Settings->SetAudioDisabled(NewCHeckedState == ECheckBoxState::Checked);
	}
	else if (inOption == EBoolOptions::Tutorial)
	{
		Settings->SetTutorialDisabled(NewCHeckedState == ECheckBoxState::Checked);
	}
}

void FToxicUtilitiesModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FToxicUtilitiesCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FToxicUtilitiesCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FToxicUtilitiesModule, ToxicUtilities)