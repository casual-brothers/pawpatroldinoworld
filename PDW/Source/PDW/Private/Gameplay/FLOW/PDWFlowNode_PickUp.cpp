// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_PickUp.h"
#include "Gameplay/Components/PDWPickUpComponent.h"
#include "Managers/QuestSubsystem.h"

void UPDWFlowNode_PickUp::ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	UPDWPickUpComponent* PickUpComponent = Cast<UPDWPickUpComponent>(Actor->GetComponentByClass(UPDWPickUpComponent::StaticClass()));
	if(!PickUpComponent) return;

	PickUpComponent->OnPickUpEnd.AddUniqueDynamic(this, &UPDWFlowNode_PickUp::OnPickUpEnd);
	Super::ObserveActor(Actor,Component);
}

void UPDWFlowNode_PickUp::ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	Super::ForgetActor(Actor,Component);

	UPDWPickUpComponent* PickUpComponent = Cast<UPDWPickUpComponent>(Actor->GetComponentByClass(UPDWPickUpComponent::StaticClass()));
	if(!PickUpComponent) return;

	PickUpComponent->OnPickUpEnd.RemoveDynamic(this, &UPDWFlowNode_PickUp::OnPickUpEnd);
}

void UPDWFlowNode_PickUp::OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		if(!PickUp) return;

		QuestSubsystem->RemoveQuestTarget(PickUp->GetOwner(), TargetsData);
		OnEventReceived();
	}
}
