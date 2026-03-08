#pragma once


#include "GameFramework/PlayerInput.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedActionKeyMapping.h"
#include "NebulaFlowEnhancedPlayerInput.generated.h"

UCLASS(Within = PlayerController, config = Input, transient)
class NEBULAFLOW_API UNebulaFlowEnhancedPlayerInput : public UEnhancedPlayerInput
{

	GENERATED_BODY()

public:

	UNebulaFlowEnhancedPlayerInput();
	
	virtual bool InputKey(const FInputKeyEventArgs& Params);

	bool IsKeyBoundToAction (FKey Key, UInputAction* Action) const;

	bool IsPlayerUsingGamepad(){ return bIsUsingGamepad; }

	const TArray<FEnhancedActionKeyMapping> GetEnhancedInputActionMappings();

private:

	bool bIsUsingGamepad;

};