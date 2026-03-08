// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "PDWGraphicCustomizationComponent.generated.h"

/**
 * 
 */
 
USTRUCT(BlueprintType)
struct FPDWUICustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FInstancedStruct UIData;
};


USTRUCT(BlueprintType)
struct FPDWUICustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FPDWUICustomizeData UICustomizationData;
};

 UCLASS()
class PDW_API UPDWCustomizationDataAsset : public UCustomizationDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FPDWUICustomization> UICustomizationList = {};
};