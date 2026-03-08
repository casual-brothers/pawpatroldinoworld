// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWSoundtrackNode.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWSoundtrackNode : public UFlowNode
{
	GENERATED_BODY()
	

public:

	UPDWSoundtrackNode(const FObjectInitializer& ObjectInitializer);

	void ExecuteInput(const FName& PinName) override;

protected:

	virtual void PlaySoundtrack();

	static FName INPIN_NotifyContainer;

	FGameplayTagContainer ResolveNotifyContainer() const;

	UPROPERTY()
	FGameplayTagContainer ResolvedNotifyContainer;

	UPROPERTY(EditAnywhere, Category = "Notify",meta =(DefaultForInputFlowPin, FlowPinType = GameplayTagContainer))
	FGameplayTagContainer NotifyTags;

};
