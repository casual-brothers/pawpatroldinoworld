// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Actions/NebulaInteractionBehaviourActions.h"

struct NEBULAINTERACTIONSYSTEMEDITOR_API FInteractionAssetCategoryPaths : EAssetCategoryPaths
{
	static FAssetCategoryPath InteractionSystem;
};

class NEBULAINTERACTIONSYSTEMEDITOR_API FNebulaInteractionSystemEditorModule : public IModuleInterface
{
public:

	static EAssetTypeCategories::Type InteractionAssetCategory;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	
	TSharedPtr<FNebulaInteractionBehaviourActions> NebulaInteractionBehaviourActions;

	void RegisterAssets();
};
