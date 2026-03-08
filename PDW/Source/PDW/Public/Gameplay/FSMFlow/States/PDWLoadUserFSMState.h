// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/States/NebulaFlowLoadUserFSMState.h"
#include "PDWLoadUserFSMState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWLoadUserFSMState : public UNebulaFlowLoadUserFSMState
{
	GENERATED_BODY()
	
protected:
	void OnUserLoaded(int ControllerId) override;

};
