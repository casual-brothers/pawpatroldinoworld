// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_UnlockDinoPen.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Unlock Dino Pen"))
class PDW_API UPDWFlowNode_UnlockDinoPen : public UFlowNode
{
	GENERATED_BODY()

public:
	UPDWFlowNode_UnlockDinoPen(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

	UPROPERTY(EditAnywhere)
	FGameplayTag DinoPenTag;

	UPROPERTY(EditAnywhere)
	bool StartInactive = false;

protected:
	
	UFUNCTION()
	void UnlockDinoPen();

	virtual void ExecuteInput(const FName& PinName) override;


	void OnPassThrough_Implementation() override;

};
