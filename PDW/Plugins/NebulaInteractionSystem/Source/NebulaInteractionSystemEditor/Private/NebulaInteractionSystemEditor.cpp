// Copyright Epic Games, Inc. All Rights Reserved.

#include "NebulaInteractionSystemEditor.h"

#define LOCTEXT_NAMESPACE "FNebulaInteractionSystemEditorModule"

EAssetTypeCategories::Type FNebulaInteractionSystemEditorModule::InteractionAssetCategory = static_cast<EAssetTypeCategories::Type>(0);
FAssetCategoryPath FInteractionAssetCategoryPaths::InteractionSystem(LOCTEXT("InteractionSystem", "InteractionSystem"));


void FNebulaInteractionSystemEditorModule::StartupModule()
{
	RegisterAssets();
}

void FNebulaInteractionSystemEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return;
	}
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(NebulaInteractionBehaviourActions.ToSharedRef());
}

void FNebulaInteractionSystemEditorModule::RegisterAssets()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// try to merge asset category with a built-in one
	{
		const FText AssetCategoryText = FText::FromString("InteractionSystem");

		// Find matching built-in category
		if (!AssetCategoryText.IsEmpty())
		{
			TArray<FAdvancedAssetCategory> AllCategories;
			AssetTools.GetAllAdvancedAssetCategories(AllCategories);
			for (const FAdvancedAssetCategory& ExistingCategory : AllCategories)
			{
				if (ExistingCategory.CategoryName.EqualTo(AssetCategoryText))
				{
					InteractionAssetCategory = ExistingCategory.CategoryType;
					break;
				}
			}
		}

		if (InteractionAssetCategory == EAssetTypeCategories::None)
		{
			InteractionAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("InteractionSystem")), AssetCategoryText);
		}
	}

	NebulaInteractionBehaviourActions = MakeShared<FNebulaInteractionBehaviourActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(NebulaInteractionBehaviourActions.ToSharedRef());
	/*const TSharedRef<IAssetTypeActions> FlowNodeActions = MakeShareable(new FNebulaInteractionBehaviourActions());
	RegisteredAssetActions.Add(FlowNodeActions);
	AssetTools.RegisterAssetTypeActions(FlowNodeActions);

	const TSharedRef<IAssetTypeActions> FlowNodeAddOnActions = MakeShareable(new FAssetTypeActions_FlowNodeAddOnBlueprint());
	RegisteredAssetActions.Add(FlowNodeAddOnActions);
	AssetTools.RegisterAssetTypeActions(FlowNodeAddOnActions);*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNebulaInteractionSystemEditorModule, NebulaInteractionSystemEditor)