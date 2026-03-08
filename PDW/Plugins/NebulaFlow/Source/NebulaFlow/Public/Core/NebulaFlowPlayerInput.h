
#pragma once


#include "GameFramework/PlayerInput.h"
#include "NebulaFlowPlayerInput.generated.h"


UCLASS(Within = PlayerController, config = Input, transient)
class NEBULAFLOW_API UNebulaFlowPlayerInput : public UPlayerInput
{

	GENERATED_BODY()

public:

	UNebulaFlowPlayerInput();

	virtual bool InputKey(const FInputKeyEventArgs& Params);
	
	bool IsKeyBoundToAction (FKey Key, FName ActionName) const;

	bool IsPlayerUsingGamepad(){ return bIsUsingGamepad; }

private:

	bool bIsUsingGamepad;

};