#include "Actions/NebulaInteractionBehaviourActions.h"

#include "Behaviours/NebulaInteractionBehaviour.h"
#include "Blueprint/InteractionBehaviourBlueprint.h"
#include "NebulaInteractionSystemEditor.h"
#include "Factory/NebulaInteractionBehaviourFactory.h"

UClass* FNebulaInteractionBehaviourActions::GetSupportedClass() const
{
    return UInteractionBehaviourBlueprint::StaticClass();
}

FText FNebulaInteractionBehaviourActions::GetName() const
{
    return INVTEXT("Interaction Behaviour");
}

FColor FNebulaInteractionBehaviourActions::GetTypeColor() const
{
    return FColor(170,245,95);
}

uint32 FNebulaInteractionBehaviourActions::GetCategories()
{
    return FNebulaInteractionSystemEditorModule::InteractionAssetCategory;
}

UFactory* FNebulaInteractionBehaviourActions::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UNebulaInteractionBehaviourFactory* InteractionBehaviourFactory = NewObject<UNebulaInteractionBehaviourFactory>();
	InteractionBehaviourFactory->ParentClass = TSubclassOf<UNebulaInteractionBehaviour>(*InBlueprint->GeneratedClass);
	return InteractionBehaviourFactory;
}

bool FNebulaInteractionBehaviourActions::CanCreateNewDerivedBlueprint() const
{
    return true;
}
