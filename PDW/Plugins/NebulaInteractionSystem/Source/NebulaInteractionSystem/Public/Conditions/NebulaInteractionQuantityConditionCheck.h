#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"

#include "NebulaInteractionConditionCheck.h"
#include "NebulaInteractionQuantityConditionCheck.generated.h"

UENUM(BlueprintType)
enum class ENebulaConditionRule : uint8
{
	Less =				0,
	Greater =			1,
	LessOrEqual =		2,
	GreaterOrEqual =	3,
	Equal =				4,
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionQuantityConditionCheck : public UNebulaInteractionConditionCheck
{
	GENERATED_BODY()
	
public:

	//************************************
	// Method:    EvaluateCondition
	// FullName:  UNebulaInteractionQuantityConditionCheck::EvaluateCondition
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const FNebulaInteractionPayload & inPayload
	// Parameter: const FNebulaInteractReceiverPayload & inReceiverPayload
	// Description: Evaluates the condition based on the current quantity and the target quantity.
	//************************************
	//virtual bool EvaluateCondition(const FNebulaInteractionPayload& inPayload, const FNebulaInteractReceiverPayload& inReceiverPayload);

protected:
	UPROPERTY(EditAnywhere)
	ENebulaConditionRule ConditionRule = ENebulaConditionRule::Equal;

	UPROPERTY(EditAnywhere)
	uint8 TargetQuantity = 0;

private:

	uint8 CurrentQuantity = 0;
};
