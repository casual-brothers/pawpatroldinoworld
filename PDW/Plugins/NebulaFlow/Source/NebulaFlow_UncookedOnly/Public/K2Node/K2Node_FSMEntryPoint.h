#pragma once

#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"

#include "K2Node_FSMEntryPoint.generated.h"

class ITargetPlatform;
struct FPropertyChangedEvent;

UCLASS()
class UK2Node_FSMEntryPoint : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()

public:

	static const FName DefaultOutputPinName;

	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FName> PinNames;

	/* Begin UObject interface */

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostEditUndo() override;

	/* End of UObject interface */

	/* Begin UEdGraphNode Interface */

	virtual void AllocateDefaultPins() override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual bool CanDuplicateNode() const override;

	virtual FText GetTooltipText() const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	/* End UEdGraphNode Interface */

	/* Begin UK2Node Interface */

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual bool ShouldShowNodeProperties() const override;

	virtual FText GetMenuCategory() const override;

	/* End UK2Node Interface */

	/* Begin IK2Node_AddPinInterface Interface */

	// Adds a new execution pin to a switch node
	virtual void AddInputPin() override;

	/* End IK2Node_AddPinInterface Interface */

	// Updates DefaultLabelName with first connected EntryPoint
	void UpdateDefaultLabel();

private:

	// Adds or removes transition to entry point map
	void UpdateTransitions(UEdGraphPin* Pin);
};