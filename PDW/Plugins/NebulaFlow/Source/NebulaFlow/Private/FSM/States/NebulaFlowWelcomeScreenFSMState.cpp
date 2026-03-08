#include "FSM/States/NebulaFlowWelcomeScreenFSMState.h"

UNebulaFlowWelcomeScreenFSMState::UNebulaFlowWelcomeScreenFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNebulaFlowWelcomeScreenFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action.Equals(ProceedAction, ESearchCase::IgnoreCase))
	{
		TriggerTransition(ProceedTransition.GetTagName());
	}
}

