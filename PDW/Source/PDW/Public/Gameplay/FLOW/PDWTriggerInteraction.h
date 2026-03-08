// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWTriggerInteraction.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Activate Interaction"))
class PDW_API UPDWTriggerInteraction : public UFlowNode
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif

protected:

	virtual void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere, Category = "Notify")
	EGameplayContainerMatchType MatchType;

	/**
 * If true, identity tags must be an exact match.
 * Be careful, setting this to false may be very expensive, as the
 * search cost is proportional to the number of registered Gameplay Tags!
 */
	UPROPERTY(EditAnywhere, Category = "Notify")
	bool bExactMatch;

	UPROPERTY(EditAnywhere, Category = "Notify")
	FGameplayTagContainer IdentityTags;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FGameplayTag InteractionTag;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool ShouldRemoveInteractionWhenFinished = false;
};
