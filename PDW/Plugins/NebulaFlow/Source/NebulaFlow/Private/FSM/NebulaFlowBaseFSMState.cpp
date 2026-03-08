// Copyright Epic Games, Inc. All Rights Reserved.
#include "FSM/NebulaFlowBaseFSMState.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "FSM/StateInitConditions/NebulaFlowBaseStateInitCondition.h"


UNebulaFlowBaseFSMState::UNebulaFlowBaseFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowBaseFSMState::OnFSMStateEnter_Implementation(const FString& InOption)
{
	AudioManagerRef = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(this);
	if (AudioManagerRef)
	{
		for (FFsmAudioOperation& currentOperation : AudioOperations)
		{
			if (!currentOperation.bOnExitState)
			{
				AudioManagerRef->ExecuteAudioOperation(currentOperation);
			}
		}
	}
}

void UNebulaFlowBaseFSMState::OnFSMStateExit_Implementation()
{
	if (AudioManagerRef)
	{
		for (FFsmAudioOperation& currentOperation : AudioOperations)
		{
			if (currentOperation.bOnExitState)
			{
				AudioManagerRef->ExecuteAudioOperation(currentOperation);
			}
		}
	}
}

void UNebulaFlowBaseFSMState::OnFSMStateUpdate_Implementation(float deltaTime)
{
	if (bIsExiting)
	{
		return;
	}
}

void UNebulaFlowBaseFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	if (bIsExiting)
	{
		return;
	}
}

void UNebulaFlowBaseFSMState::TriggerTransition(const FName TransitionName,const FName LabelName)
{
	if (OwnerFSM && !bIsExiting)
	{
		bIsExiting = OwnerFSM->TriggerTransition(TransitionName,LabelName);
	}
}

void UNebulaFlowBaseFSMState::TriggerTransitionWithOption(const FName TransitionName, FString InOption, const FName LabelName /*= NAME_None*/)
{
	if (OwnerFSM && !bIsExiting)
	{
		bIsExiting = OwnerFSM->TriggerTransitionWithOption(TransitionName, InOption ,LabelName);
	}
}

void UNebulaFlowBaseFSMState::TriggerLabelTransition(const FName LabelName, FString InOption /*= FString("")*/)
{
	if (OwnerFSM && !bIsExiting)
	{
		bIsExiting = OwnerFSM->TriggerLabelTransition(LabelName, InOption);
	}
}

UWorld* UNebulaFlowBaseFSMState::GetWorld() const
{
	UNebulaFlowBaseFSM* StateOwner = Cast<UNebulaFlowBaseFSM>(GetOuter());
	if (StateOwner)
	{
		return StateOwner->GetWorld();
	}
	return nullptr;
}

bool UNebulaFlowBaseFSMState::HasInitConditions()
{
	return InitConditionsClasses.Num()>0;
}

bool UNebulaFlowBaseFSMState::HasPendingConditions()
{
	return _InternalConditionsObjects.Num()>0;
}

void UNebulaFlowBaseFSMState::InitState(FName InStateName, UNebulaFlowBaseFSM* InOwner)
{
	GetWorld();
	StateName = InStateName;
	OwnerFSM = InOwner;

	OnActionDelegateHandler = FNebulaFlowCoreDelegates::OnActionTriggered.AddUObject(this,&UNebulaFlowBaseFSMState::OnFSMStateAction);

	for(auto* currentBehavior : StateBehaviors)
	{
		if(currentBehavior)
		{
			currentBehavior->InitStateBehavior(this);
		}
	}

	if(HasInitConditions())
	{
		for(auto InitConditionClass : InitConditionsClasses)
		{ 
			_InternalConditionsObjects.Add(NewObject<UNebulaFlowBaseStateInitCondition>(this, InitConditionClass));
		}
	}
}

void UNebulaFlowBaseFSMState::UnInitState()
{
	FNebulaFlowCoreDelegates::OnActionTriggered.Remove(OnActionDelegateHandler);
}
