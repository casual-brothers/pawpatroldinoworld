#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "NebulaInteractionBehaviourFactory.generated.h"

UCLASS(HideCategories = Object)
class NEBULAINTERACTIONSYSTEMEDITOR_API UNebulaInteractionBehaviourFactory : public UFactory
{
    GENERATED_BODY()
public:
	UNebulaInteractionBehaviourFactory(const FObjectInitializer& ObjectInitializer);
	// The Default parent class of the created blueprint (set by subclasses)
	UPROPERTY()
	TSubclassOf<class UNebulaInteractionBehaviour> DefaultParentClass;
		
	// The parent class of the created blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNebulaInteractionBehaviour> ParentClass;

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateNew(UClass* BlueprintClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	void ShowCannotCreateBlueprintDialog();
protected:
	// Parameterized guts of ConfigureProperties()
	/*bool ConfigurePropertiesInternal(const FText& TitleText);*/
};
