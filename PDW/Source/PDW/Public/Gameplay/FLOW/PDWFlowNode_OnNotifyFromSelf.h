// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_OnNotifyFromSelf.generated.h"

/**
 * Triggers output when the owning Flow Component calls NotifyGraph function with matching Notify Tag
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "On Notify From Self"))
class PDW_API UPDWFlowNode_OnNotifyFromSelf : public UFlowNode
{
	GENERATED_UCLASS_BODY()
protected:

	static FName INPIN_NotifyContainer;

	void ExecuteInput(const FName& PinName) override;
	void OnLoad_Implementation() override;
	UFUNCTION()
	void OnNotifyFromComponent(UFlowComponent* Component, const FGameplayTag& Tag);
	void StartObserving();
	virtual void Cleanup() override;

	FGameplayTagContainer ResolveNotifyContainer() const;
#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif

protected:

	UPROPERTY()
	FGameplayTagContainer ResolvedNotifyContainer;

	UPROPERTY(EditAnywhere, Category = "Notify",meta =(DefaultForInputFlowPin, FlowPinType = GameplayTagContainer))
	FGameplayTagContainer NotifyTags;
	
	FDelegateHandle DelegateHandle;
};
