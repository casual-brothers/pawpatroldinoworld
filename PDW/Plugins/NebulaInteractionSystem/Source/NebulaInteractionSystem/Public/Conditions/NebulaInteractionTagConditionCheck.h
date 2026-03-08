#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "NebulaInteractionConditionCheck.h"

#include "NebulaInteractionTagConditionCheck.generated.h"

struct FInteractionInfo;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionTagConditionCheck : public UNebulaInteractionConditionCheck
{
	GENERATED_BODY()
	
public:

	//************************************
	// Method:    EvaluateCondition
	// FullName:  UNebulaInteractionTagConditionCheck::EvaluateCondition
	// Access:    virtual public 
	// Returns:   bool
	// Parameter: const FInteractionInfo & inPayload
	// Description: Evaluates the condition based on the provided payloads and the tag query.
	//************************************
	virtual bool EvaluateCondition(const FInteractionInfo& inPayload);

protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTagQuery Query;
};
