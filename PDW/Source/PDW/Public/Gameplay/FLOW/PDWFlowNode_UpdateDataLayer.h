// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_UpdateDataLayer.generated.h"

class UDataLayerAsset;
/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Update Data Layer", Keywords = "layer"))
class PDW_API UPDWFlowNode_UpdateDataLayer : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:
	void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<UDataLayerAsset>, bool> DataLayersInfo {};

	UFUNCTION()
	void UpdateDataLayer();

	void OnPassThrough_Implementation() override;

};
