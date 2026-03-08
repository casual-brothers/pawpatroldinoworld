// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "PDWDialogueFlowAsset.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueCompleted);

UCLASS()
class PDW_API UPDWDialogueFlowAsset : public UFlowAsset
{
	GENERATED_BODY()

	UPDWDialogueFlowAsset();

public:
	
	FOnDialogueCompleted OnDialogueCompleted;

	UFUNCTION(BlueprintCallable)
	void SetEntryPoint(const FGameplayTag& inEntryPoint);
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetEntryPoint() const;

protected:
	FGameplayTag CurrentEntryPoint = FGameplayTag::EmptyTag;
private:
	void OnDialogueUIReady(IFlowDataPinValueSupplierInterface* DataPinValueSupplier);
};
