// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "PDWMinigameFlowAsset.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FPDWMinigameFlowAssetConfiguration
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag MiniGameTag = FGameplayTag::EmptyTag;
};

UCLASS()
class PDW_API UPDWMinigameFlowAsset : public UFlowAsset
{
	GENERATED_BODY()

public:

	void SetMinigameTag(const FGameplayTag& inMinigameTag);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetMinigameTag() const;

protected:

	UPROPERTY()
	FPDWMinigameFlowAssetConfiguration Configuration;
};
