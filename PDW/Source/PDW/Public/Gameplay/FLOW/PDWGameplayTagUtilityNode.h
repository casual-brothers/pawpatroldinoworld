// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWGameplayTagUtilityNode.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "GameplayTagContainer Check"))
class PDW_API UPDWGameplayTagUtilityNode : public UFlowNode
{
	GENERATED_BODY()
	
	UPDWGameplayTagUtilityNode(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTagQuery Query;

	UPROPERTY(EditAnywhere)
	FFlowDataPinInputProperty_GameplayTagContainer InContainer;

	static FName OUTPIN_False;
	static FName OUTPIN_True;
	static FName INPIN_TagContainerToCheck;

	FGameplayTagContainer ResolveNotifyContainer() const;	
public:
	void ExecuteInput(const FName& PinName) override;

};
