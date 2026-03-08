// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "NebulaFlowLoadLevelFSMState.generated.h"


UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowLoadLevelFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

protected:

	UPROPERTY(EditAnywhere, Category ="Flow")
	FName LevelToLoadName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Flow")
	bool bIsabsolute = false;

	UPROPERTY(EditAnywhere, Category = "Flow")
	FString TravelOptions = FString("");
};