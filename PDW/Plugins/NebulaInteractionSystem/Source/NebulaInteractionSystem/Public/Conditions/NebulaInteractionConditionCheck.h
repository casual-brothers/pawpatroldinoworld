#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Data/NebulaInteractionSystemStructs.h"
#include "Components/InteractionFlowComponent.h"

#include "NebulaInteractionConditionCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionConditionCheck : public UObject
{
	GENERATED_BODY()
	
public:

	virtual bool EvaluateCondition(const FInteractionInfo& inPayload){return true;}
};
