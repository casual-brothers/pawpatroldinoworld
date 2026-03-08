
#include "Core/NebulaFlowEnhancedPlayerInput.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "EnhancedActionKeyMapping.h"




UNebulaFlowEnhancedPlayerInput::UNebulaFlowEnhancedPlayerInput()
:Super()
{

}

bool UNebulaFlowEnhancedPlayerInput::InputKey(const FInputKeyEventArgs& Params)
{
	FKey Key = Params.Key;
	EInputEvent Event = Params.Event;
	bool bGamepad = Params.IsGamepad();

	if (Event == EInputEvent::IE_Released)
	{
		FNebulaFlowCoreDelegates::OnAnyInputReleased.Broadcast(Key, bGamepad, Cast<APlayerController>(GetOuter()));
	}

	if (Event == EInputEvent::IE_Pressed)
	{
		FNebulaFlowCoreDelegates::OnAnyInputPressed.Broadcast(Key, bGamepad, Cast<APlayerController>(GetOuter()));
	}

	bIsUsingGamepad = bGamepad;

	return Super::InputKey(Params);
}

bool UNebulaFlowEnhancedPlayerInput::IsKeyBoundToAction(FKey Key,UInputAction* Action) const
{
	if (!ensureMsgf(Action, TEXT("No Action Found")))
	{
		return false;
	}
	if (IsKeyHandledByAction(Key))
	{		
		const TArray<FEnhancedActionKeyMapping> CurrentActionMappings = GetEnhancedActionMappings();
		for (FEnhancedActionKeyMapping currentMapping : CurrentActionMappings)
		{
			if (currentMapping.Key == Key && currentMapping.Action->GetName().Equals(Action->GetName()))
			{
				return true;
			}
		}		
	}
	return false;
}

const TArray<FEnhancedActionKeyMapping> UNebulaFlowEnhancedPlayerInput::GetEnhancedInputActionMappings()
{
	return GetEnhancedActionMappings();
}