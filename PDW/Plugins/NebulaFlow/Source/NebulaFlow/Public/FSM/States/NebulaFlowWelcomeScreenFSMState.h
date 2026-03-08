// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NebulaFlowBaseUIFSMState.h"
#include "GameplayTagContainer.h"
#include "NebulaFlowWelcomeScreenFSMState.generated.h"


UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowWelcomeScreenFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender);

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	FString ProceedAction = "ProceedToMenu";

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	FGameplayTag ProceedTransition;

};