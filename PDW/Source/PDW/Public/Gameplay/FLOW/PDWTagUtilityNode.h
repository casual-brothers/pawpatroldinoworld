// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWTagUtilityNode.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "GameplayTag Check"))
class PDW_API UPDWTagUtilityNode : public UFlowNode
{
	GENERATED_BODY()
	
	UPDWTagUtilityNode(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTagContainer ContainerToCheck;

	UPROPERTY(EditAnywhere)
	FFlowDataPinInputProperty_GameplayTag InTag;

	static FName OUTPIN_False;
	static FName OUTPIN_True;
	static FName INPIN_TagToCheck;

	FGameplayTag ResolveGameplayTag() const;	
public:
	void ExecuteInput(const FName& PinName) override;
	
};
