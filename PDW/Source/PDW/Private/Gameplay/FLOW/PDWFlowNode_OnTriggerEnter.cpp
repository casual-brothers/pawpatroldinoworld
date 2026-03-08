// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_OnTriggerEnter.h"
#include "Managers/QuestSubsystem.h"
#include "FlowComponent.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWFlowNode_OnTriggerEnter::ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	if(IdentityTags.HasTagExact(Component->IdentityTags.First()))
	{
		UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
		if (EventSubsystem)
		{
			EventSubsystem->OnObservedActorLoadedEvent(Actor.Get(), Component.Get(), TargetsData, this);
		}
	}

	Super::ObserveActor(Actor,Component);
}

void UPDWFlowNode_OnTriggerEnter::ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	if(IdentityTags.HasTagExact(Component->IdentityTags.First()))
	{
		UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
		if (EventSubsystem)
		{
			EventSubsystem->OnObservedActorUnloadedEvent(Actor.Get(), Component.Get(), TargetsData, this);
		}
	}

	Super::ForgetActor(Actor,Component);
}

void UPDWFlowNode_OnTriggerEnter::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (PinName == TEXT("Start"))
	{
		SetTargets();
	}
}

void UPDWFlowNode_OnTriggerEnter::OnTriggerEvent(const bool bOverlapping, UFlowComponent* OtherFlowComponent, UFlowComponent* SourceFlowComponent)
{
	if (OtherFlowComponent && SourceFlowComponent && bReactOnOverlapping == bOverlapping && OtherFlowComponent->IdentityTags.HasAnyExact(OverlappedActorTags))
	{
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			QuestSubsystem->RemoveQuestTarget(SourceFlowComponent->GetOwner(), TargetsData);
		}

		OnEventReceived();
	}
}

void UPDWFlowNode_OnTriggerEnter::SetTargets()
{
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		TargetsData.CurrentQuantity = SuccessCount;
		TargetsData.TargetQuantity = SuccessLimit;
		TargetsData.IdentityTag = GetIdentityTags();
		QuestSubsystem->SetQuestTargets(TargetsData);
	}
}

void UPDWFlowNode_OnTriggerEnter::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	SetTargets();
}
