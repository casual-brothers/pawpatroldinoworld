// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "PDWFlowNode_OnAbilityActivated.generated.h"

class UGameplayAbility;
class UPDWGASAbility;
class UPDWGASComponent;
/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Wait For Ability"))
class PDW_API UPDWFlowNode_OnAbilityActivated : public UFlowNode
{
	GENERATED_UCLASS_BODY()
public:
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
protected:

	virtual void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	void OnAbilityActivated(UGameplayAbility* Data);

	UFUNCTION()
	void OnAbilityEnd(FAbilityEndedData Data);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWGASAbility> AbilityToCheck{};

	FDelegateHandle OnAbilityActivatedHandle;
	FDelegateHandle OnAbilityEndedHandle;

	UPROPERTY()
	TObjectPtr<UPDWGASComponent> GasComp;
};
