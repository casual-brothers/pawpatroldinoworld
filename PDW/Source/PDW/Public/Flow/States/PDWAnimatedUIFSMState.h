// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "PDWAnimatedUIFSMState.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWAnimatedUIFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption /* = FString("") */) override;
	virtual void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender);

	UFUNCTION()
	void OnAnimationFinished();

protected:
	UPROPERTY(EditAnywhere)
	bool bIsEnableTabNavigation = false;
	
	UPROPERTY(EditAnywhere)
	bool bSkippable;
};
