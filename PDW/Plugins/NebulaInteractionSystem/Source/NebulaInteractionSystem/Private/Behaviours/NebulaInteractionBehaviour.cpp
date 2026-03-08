#include "Behaviours/NebulaInteractionBehaviour.h"
#include "Components/NebulaInteractionReceiverComponent.h"
#include "Components/NebulaInteractionComponent.h"
#include "NebulaInteractionSystem.h"
#include "Interfaces/InteractableInterface.h"

#pragma region CORE

void UNebulaInteractionBehaviour::InitBehaviour(const FBehaviourInfo& inBehaviourInfo)
{
	Interactor = inBehaviourInfo.Interactor;
	Owner = inBehaviourInfo.InteractionBehaviourOwner;
	InitBehaviour_Implementation(inBehaviourInfo);
	BP_InitBehaviour_Implementation(inBehaviourInfo);
	bTickEnabled = true;
}

void UNebulaInteractionBehaviour::ExecuteBehaviour()
{
	BroadcastEvent(EBroadcastType::Start);
	ExecuteBehaviour_Implementation();
	BP_ExecuteBehaviour_Implementation();
	NotifySuccess();
}

void UNebulaInteractionBehaviour::TickBehaviour(const float inDeltaTime)
{
	TickBehaviour_Implementation(inDeltaTime);
	BP_TickBehaviour_Implementation(inDeltaTime);
}

void UNebulaInteractionBehaviour::InterruptBehaviour(AActor* inInterruptionSource)
{
	BroadcastEvent(EBroadcastType::Interrupt,inInterruptionSource);
	StopBehaviour(true);
}

void UNebulaInteractionBehaviour::StopBehaviour(bool bInterrupted /*=false*/)
{
	StopBehaviour_Implementation(bInterrupted);
	BP_StopBehaviour_Implementation(bInterrupted);
	if(!bInterrupted)
	{
		BroadcastEvent(EBroadcastType::End);
	}
	bTickEnabled=false;
	Owner.Reset();
	Interactor.Reset();
}

void UNebulaInteractionBehaviour::UpdateInteractableState() const
{
	if (BehaviourConfiguration.InteractableTransitionToState.IsValid())
	{
		IInteractableInterface* InteractionInterface = Cast<IInteractableInterface>(Owner.Get());
		if (InteractionInterface)
		{
			InteractionInterface->SetInteractionState(BehaviourConfiguration.InteractableTransitionToState);
		}
		else if (Owner.Get()->Implements<UInteractableInterface>())
		{
			IInteractableInterface::Execute_SetInteractionState(Owner.Get(),BehaviourConfiguration.InteractableTransitionToState);
		}
		else
		{
			UE_LOG(NebulaInteractionLog,Warning,TEXT("NebulaInteractionBehaviour - Couldn't cast to state interface"));
		}
	}
}

void UNebulaInteractionBehaviour::UpdateInteractorStatus() const
{
	if (BehaviourConfiguration.InteractorStatusAdded.Num() || BehaviourConfiguration.InteractorStatusRemoved.Num())
	{
		IInteractableInterface* InteractionInterface = Cast<IInteractableInterface>(Interactor.Get());
		if (InteractionInterface)
		{
			if (BehaviourConfiguration.InteractorStatusAdded.Num())
			{
				InteractionInterface->AddInteractionStatus(BehaviourConfiguration.InteractorStatusAdded);
			}
			if (BehaviourConfiguration.InteractorStatusRemoved.Num())
			{
				InteractionInterface->RemoveInteractionStatus(BehaviourConfiguration.InteractorStatusRemoved);
			}
		}
		else
		{
			UE_LOG(NebulaInteractionLog,Warning,TEXT("NebulaInteractionBehaviour - Couldn't cast to state interface"));
		}
	}
}

void UNebulaInteractionBehaviour::NotifySuccess()
{
	if (BehaviourConfiguration.bNotifySuccess)
	{
		//IMPLEMENT YOUR NOTIFICATION LOGIC HERE

		/*FInteractionEventSignature InteractionEvent;
		InteractionEvent.EventTag = BehaviourConfiguration.EventID;
		InteractionEvent.Interacter = BehaviourExecutioner;
		UPDWEventSubsytem::Get(BehaviourExecutioner)->TriggerInteractionEvent(BehaviourConfiguration.TargetIDToNotify,InteractionEvent);*/
	}
}

void UNebulaInteractionBehaviour::BroadcastEvent(const EBroadcastType& inType, AActor* InterruptSource /*= nullptr*/) const
{
	if (inType == EBroadcastType::Interrupt)
	{
		FInteractionBehaviourInterruptInfo Info;
		Info.InteractionBehaviourID = BehaviourConfiguration.BehaviourID;
		Info.InteractionBehaviourOwner = Owner.Get();
		Info.Interactor = Interactor.Get();
		Info.InterruptionSource = InterruptSource;
		OnBehaviourInterruped.Broadcast(Info);
	}
	else
	{
		FInteractionBehaviourEventInfo Info;
		Info.InteractionBehaviourID = BehaviourConfiguration.BehaviourID;
		Info.InteractionBehaviourOwner = Owner.Get();
		Info.Interactor = Interactor.Get();
		if (inType == EBroadcastType::Start)
		{
			OnBehaviourStarted.Broadcast(Info);
		}
		else if (inType == EBroadcastType::End)
		{
			OnBehaviourFinished.Broadcast(Info);
		}
	}
}

#pragma endregion

#pragma region GETTERS

FGameplayTag UNebulaInteractionBehaviour::GetInteractionBehaviourID() const
{
	return BehaviourConfiguration.BehaviourID;
}

AActor* UNebulaInteractionBehaviour::GetOwner() const
{
	return Owner.Get();
}

AActor* UNebulaInteractionBehaviour::GetInteractor() const
{
	return Interactor.Get();
}

void UNebulaInteractionBehaviour::Tick(float DeltaTime)
{
	TickBehaviour(DeltaTime);
}

TStatId UNebulaInteractionBehaviour::GetStatId() const
{
	return TStatId();
}

bool UNebulaInteractionBehaviour::IsTickable() const
{
	return bTickEnabled;
}

#pragma endregion

#pragma region SETTERS
#pragma endregion


