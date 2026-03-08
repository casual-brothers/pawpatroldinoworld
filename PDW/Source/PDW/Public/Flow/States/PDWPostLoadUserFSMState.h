// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "PDWPostLoadUserFSMState.generated.h"

class UEnhancedInputPlatformData;

UCLASS(MinimalAPI, Abstract)
class UPDWPostLoadUserFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_BODY()

public:
	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UEnhancedInputPlatformData> SwitchData{ nullptr };

};
