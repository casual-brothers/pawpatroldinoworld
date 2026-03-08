#pragma once

#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"
#include "GameplayTagContainer.h"

#include "K2Node_FSMState.generated.h"

struct FPropertyChangedEvent;
struct FGameplayTag;

UCLASS()
class UK2Node_FSMState : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()

public:

	static const FName DefaultOutputPinName;
	static const FName FSMStateLabelPinName;
	static const FName FSMStateClassPinName;
	static const FName FSMStateParameterPinName;
	static const FName FSMDefaultPinTag;

	UPROPERTY(EditAnywhere, Category = PinOptions)
	FGameplayTag LabelTag;

	UPROPERTY(VisibleDefaultsOnly, Category = PinOptions)
	TArray<FName> PinNames;

	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FGameplayTag> PinTagsContainer;

	/* Begin UObject interface */

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostEditUndo() override;

	/* End of UObject interface */

	/* Begin UEdGraphNode Interface */

	virtual void AllocateDefaultPins() override;

	virtual void ReconstructNode() override;

	virtual void PostPasteNode() override;

	virtual void DestroyNode() override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

	virtual FText GetTooltipText() const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	/* End UEdGraphNode Interface */

	/* Begin UK2Node Interface */

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual void EarlyValidation(class FCompilerResultsLog& MessageLog) const override;

	virtual bool ShouldShowNodeProperties() const override;

	virtual FText GetMenuCategory() const override;

	/* End UK2Node Interface */

	/* Begin IK2Node_AddPinInterface Interface */

	// Adds a new execution pin to a switch node
	virtual void AddInputPin() override;

	/* End IK2Node_AddPinInterface Interface */

	FName GetLabel() const;

private:

	FName Label = NAME_None;

	FName Parameter = NAME_None;

	void GetStateLabelsInGraph(TArray<FName>& InLabelArray) const;

	// Adds or removes transition to entry point map
	void UpdateTransitions(UEdGraphPin* Pin);

	// Updates input pins value
	void UpdateInputPinsValue(UEdGraphPin* Pin);
	
	void ValidateLabel();
};