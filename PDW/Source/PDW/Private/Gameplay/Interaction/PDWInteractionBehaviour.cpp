// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWInteractionBehaviour::InitBehaviour(USceneComponent* NewOwnerComponent, FGameplayTag NewBehaviourIdentifier)
{
	OwnerComponent = NewOwnerComponent;
	BehaviourIdentifier = NewBehaviourIdentifier;
	InitBehaviour_Implementation(NewOwnerComponent);
	BP_InitBehaviour_Implementation(NewOwnerComponent);
}

void UPDWInteractionBehaviour::ExecuteBehaviour(AActor* Executioner /*= nullptr*/ , bool bRemoveBehaviourWhenFinished /*= false*/)
{
	//OwnerComponent->SetCurrentBehaviour(this);
	CurrentBehaviourElapsedTime = 0;
	BehaviourProgressPercentage = 0;
	ShouldRemoveBehaviourWhenFinished = bRemoveBehaviourWhenFinished;
	BehaviourExecutioner = Executioner;
	ExecuteBehaviour_Implementation();
	BP_ExecuteBehaviour_Implementation();
	UpdateInteracterStates();
}

void UPDWInteractionBehaviour::SetExecutioner(AActor* inExecutioner)
{
	BehaviourExecutioner = inExecutioner;
}

void UPDWInteractionBehaviour::TickBehaviour(float DeltaTime)
{
	TickBehaviour_Implementation(DeltaTime);
	BP_TickBehaviour_Implementation(DeltaTime);

	CurrentBehaviourElapsedTime += DeltaTime;
	if (BehaviourDuration > 0)
	{
		BehaviourProgressPercentage = FMath::Clamp(CurrentBehaviourElapsedTime / BehaviourDuration, 0.0f, 1.0f);
		if(CurrentBehaviourElapsedTime > BehaviourDuration)
			StopBehaviour();
	}
}

void UPDWInteractionBehaviour::SetSkipSteps(const bool bSkip)
{
	bSkipSteps = bSkip;
}

class UWorld* UPDWInteractionBehaviour::GetWorld() const
{
	if (OwnerComponent)
	{
		return OwnerComponent->GetWorld();
	}
	return nullptr;
}

void UPDWInteractionBehaviour::UpdateInteracterStates()
{
	if (InteractionStateToAdd.Num() || InteractionStateToRemove.Num())
	{
		if (BehaviourExecutioner)
		{
			UPDWInteractionComponent* InteractionComp = BehaviourExecutioner->FindComponentByClass<UPDWInteractionComponent>();
			if (InteractionComp)
			{
				if (InteractionStateToAdd.Num())
				{
					InteractionComp->AddOwnerInteractionStates(InteractionStateToAdd);
				}
				if(InteractionStateToRemove.Num())
				{
					InteractionComp->RemoveinteractionStates(InteractionStateToRemove);
				}
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("UPDWInteractionBehaviour - MISSING INTERACTION COMPONENT - SHOULD BE IMPOSSIBLE"));
			}
		}
	}
}

void UPDWInteractionBehaviour::NotifySuccess()
{
	if (bNotifySuccess)
	{
		FInteractionEventSignature InteractionEvent;
		InteractionEvent.EventTag = EventID;
		InteractionEvent.Interacter = BehaviourExecutioner;
		if (GetOwnerComponent())
		{
			InteractionEvent.MinigameActorTarget = GetOwnerComponent()->GetOwner();
		}
		UPDWEventSubsytem::Get(BehaviourExecutioner)->TriggerInteractionEvent(TargetIDToNotify,InteractionEvent);
	}
}

void UPDWInteractionBehaviour::StopBehaviour()
{
	NotifySuccess();
	StopBehaviour_Implementation();
	BP_StopBehaviour_Implementation();
	if (UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent> (OwnerComponent))
	{
		InteractionComp->BehaviourFinished(this);
		if (ShouldRemoveBehaviourWhenFinished)
		{
			InteractionComp->RemoveBehaviour(BehaviourIdentifier);
		}
	}
}

