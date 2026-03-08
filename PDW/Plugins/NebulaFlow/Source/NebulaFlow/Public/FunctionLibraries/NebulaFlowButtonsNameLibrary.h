// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NebulaFlowButtonsNameLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NEBULAFLOW_API UNebulaFlowButtonsNameLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
		static const FName SWITCH_REMAP_JOYCONS;
		static const FName XBOX_PROFILE_SWAP;

};
