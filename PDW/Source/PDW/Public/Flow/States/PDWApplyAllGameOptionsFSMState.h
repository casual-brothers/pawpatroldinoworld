// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "CoreMinimal.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "PDWApplyAllGameOptionsFSMState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWApplyAllGameOptionsFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_BODY()
	
public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;


};
