// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NebulaFlowInputDataStructures.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UMappingContextList : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, UInputMappingContext*> MappingContexts{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UInputAction*> InputActions{};
};
