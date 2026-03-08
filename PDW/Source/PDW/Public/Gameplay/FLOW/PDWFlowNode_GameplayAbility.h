// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_GameplayAbility.generated.h"

class UPDWGASAbility;
/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Add Gameplay Ability", Keywords = "gas"))
class PDW_API UPDWFlowNode_GameplayAbility : public UFlowNode
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void ExecuteInput(const FName& PinName) override;
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWGASAbility> AbilityRef{};
	UPROPERTY(EditAnywhere)
	bool bRemoveAbility = false;

	UPROPERTY(EditAnywhere, meta = (editcondition = "!bRemoveAbility"))
	bool bGrantAndActivate = false;

};
