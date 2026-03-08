// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once



#include "GameFramework/GameState.h"
#include "NebulaFlowGameState.generated.h"


UCLASS()
class NEBULAFLOW_API ANebulaFlowGameState : public AGameState
{

	GENERATED_UCLASS_BODY()

protected:

	/** Called when the state transitions to WaitingToStart */
	virtual void HandleMatchIsWaitingToStart() override;


};