// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_ChangeItemQuantity.generated.h"


UENUM(BlueprintType)
enum class EChangeItemOperation : uint8
{
	Add,
	Remove
};

UCLASS(NotBlueprintable, meta = (DisplayName = "Add/Remove Items"))
class PDW_API UPDWFlowNode_ChangeItemQuantity : public UFlowNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	EChangeItemOperation AddOrRemove = EChangeItemOperation::Add;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, int32> Items = {};

	UPROPERTY(EditAnywhere)
	bool Notify = false;

	UPROPERTY(EditAnywhere)
	bool InitWithInputPins = false;
	
	UPDWFlowNode_ChangeItemQuantity(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ExecuteInput(const FName& PinName) override;

};
