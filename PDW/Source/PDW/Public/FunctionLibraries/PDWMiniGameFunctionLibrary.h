// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "FlowComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "PDWMiniGameFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMiniGameFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	UFUNCTION(BlueprintInternalUseOnly)
	bool Prototype_FunctionCheck(const FMinigameFlowEventSignature& inEventSignature){ return true;};
#endif

	UFUNCTION(BlueprintCallable)
	bool OnMinigameComplete(const FMinigameFlowEventSignature& inEventSignature);

	UFUNCTION(BlueprintCallable)
	bool OnMiniGameReset(const FMinigameFlowEventSignature& inEventSignature);

	UFUNCTION(BlueprintCallable)
	bool OnMiniGameInitializationComplete(const FMinigameFlowEventSignature& inEventSignature);

	UFUNCTION(BlueprintCallable)
	bool OnMinigameLeft(const FMinigameFlowEventSignature& inEventSignature);

	UFUNCTION(BlueprintCallable)
	bool OnMinigameSkip(const FMinigameFlowEventSignature& inEventSignature);
};
