#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class NEBULAINTERACTIONSYSTEMEDITOR_API FNebulaInteractionBehaviourActions : public FAssetTypeActions_Blueprint
{
public:
	UClass* GetSupportedClass() const override;
	FText GetName() const override;
	FColor GetTypeColor() const override;
	uint32 GetCategories() override;
protected:
	UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;
	bool CanCreateNewDerivedBlueprint() const override;

};
