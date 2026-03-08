// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "GameplayTagContainer.h"
#include "NebulaFlowPreWelcomeScreenFSMState.generated.h"


UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowPreWelcomeScreenFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	virtual void OnFSMStateExit_Implementation() override;

private:

	void InitLocalisation();

	UPROPERTY(EditAnywhere)
	FGameplayTag ProceedTag;

	UPROPERTY(EditAnywhere)
	bool bForceEnglishInEditor = true;

};