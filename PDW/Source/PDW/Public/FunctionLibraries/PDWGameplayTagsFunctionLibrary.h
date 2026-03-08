// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "PDWGameplayTagsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWGameplayTagsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = "GameplayTagUtility")
	static FGameplayTag GetRootTagFromTag(const FGameplayTag& inTag);

	//#TODO -> Might be needed.

	//UFUNCTION(BlueprintPure, Category = "GameplayTagUtility")
	//static FGameplayTag GetRootTagFromContainer(const FGameplayTagContainer& inContainer);

	//UFUNCTION(BlueprintPure, Category = "GameplayTagUtility")
	//static FGameplayTagContainer GetRootContainerFromTag(const FGameplayTag& inTag);

	//UFUNCTION(BlueprintPure, Category = "GameplayTagUtility")
	//static FGameplayTagContainer GetRootContainerFromContainer(const FGameplayTagContainer& inContainer);
};
