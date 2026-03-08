// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWOverlapBehaviour.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"

void UPDWOverlapBehaviour::InitBehaviour(FPDWOverlapBehaviourInizializzation Inizialization)
{
	BehaviourInformation = Inizialization;
}

void UPDWOverlapBehaviour::ExecuteBehaviour()
{
	ExecuteBehaviour_Implementation();
	BP_ExecuteBehaviour_Implementation();
}

void UPDWOverlapBehaviour::OnInteractionSuccessfull()
{
	OnInteractionSuccessfull_Implementation();
	BP_OnInteractionSuccessfull_Implementation();
	if (OverlapConfiguration == (uint8)EOverlapBehaviourConfig::EndOnSuccess)
	{
		BehaviourInformation.Receiver->EndUIOverlapBehaviour(GetSenderComponent());
	}
}

void UPDWOverlapBehaviour::OnInteractionFailed()
{
	OnInteractionFailed_Implementation();
	BP_OnInteractionFailed_Implementation();
	if (OverlapConfiguration == (uint8)EOverlapBehaviourConfig::EndOnSuccess)
	{
		BehaviourInformation.Receiver->EndUIOverlapBehaviour(GetSenderComponent());
	}
}

void UPDWOverlapBehaviour::EndBehaviour()
{
	EndBehaviour_Implementation();
	BP_EndBehaviour_Implementation();
	//GetOwnerComponent()->ResetCollisions();
}

void UPDWOverlapBehaviour::TickBehaviour(float inDeltaTime)
{
	TickBehaviour_Implementation(inDeltaTime);
	BP_TickBehaviour_Implementation(inDeltaTime);
}

UPDWInteractionReceiverComponent* UPDWOverlapBehaviour::GetOwnerComponent()
{
	return BehaviourInformation.Receiver;
}

UPDWInteractionComponent* UPDWOverlapBehaviour::GetSenderComponent()
{
	return BehaviourInformation.Sender;
}
