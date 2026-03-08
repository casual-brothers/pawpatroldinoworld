// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_UpdateInteractions.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Update Interactions", Keywords = "Interacton"))
class PDW_API UPDWFlowNode_UpdateInteractions : public UFlowNode
{
	GENERATED_UCLASS_BODY()
#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif

protected:
	virtual void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere, Category = "Interactions")
	FGameplayTagContainer InteractionsToAdd;

	UPROPERTY(EditAnywhere, Category = "Interactions")
	FGameplayTagContainer InteractionsToRemove;

	UPROPERTY(EditAnywhere, Category = "Target Actor")
	EGameplayContainerMatchType MatchType;

	UPROPERTY(EditAnywhere, Category = "Target Actor")
	bool bExactMatch;

	UPROPERTY(EditAnywhere, Category = "Target Actor")
	FGameplayTagContainer IdentityTags;
};
