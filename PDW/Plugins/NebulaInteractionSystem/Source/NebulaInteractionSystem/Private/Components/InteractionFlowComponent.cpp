// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractionFlowComponent.h"

void UInteractionFlowComponent::TriggerInteraction(const FInstancedStruct& inInteractionInfo)
{
	OnInteractionTriggered.Broadcast(this, inInteractionInfo);
}

#pragma region GETTER
const FGameplayTag UInteractionFlowComponent::GetInteractionState() const
{
	return CurrentState;
}
#pragma endregion

#pragma region SETTER

void UInteractionFlowComponent::SetInteractionState(const FGameplayTag& inNewState)
{
	if (inNewState != CurrentState)
	{
		OnStateChanged.Broadcast(CurrentState,inNewState);
		CurrentState = inNewState;
	}
}

void UInteractionFlowComponent::BeginPlay()
{
	Super::BeginPlay();
	if (StartingState != FGameplayTag::EmptyTag)
	{
		//IMPLEMENT LOAD SYSTEM TO CHECK THE LAST SAVED STATE
		CurrentState = StartingState;
	}
}

#pragma endregion
