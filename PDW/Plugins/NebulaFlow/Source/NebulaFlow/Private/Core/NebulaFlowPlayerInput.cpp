#include "Core/NebulaFlowPlayerInput.h"
#include "Core/NebulaFlowCoreDelegates.h"

UNebulaFlowPlayerInput::UNebulaFlowPlayerInput()
:Super()
{

}

bool UNebulaFlowPlayerInput::InputKey(const FInputKeyEventArgs& Params)
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

bool UNebulaFlowPlayerInput::IsKeyBoundToAction(FKey Key, FName ActionName) const
{
	if (IsKeyHandledByAction(Key))
	{
		for (FInputActionKeyMapping currentMapping : ActionMappings)
		{
			if (currentMapping.Key == Key && currentMapping.ActionName == ActionName)
			{
				return true;
			}
		}
	}
	return false;
}
