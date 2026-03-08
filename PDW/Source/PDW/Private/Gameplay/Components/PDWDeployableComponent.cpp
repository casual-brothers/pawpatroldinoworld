// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWDeployableComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"

// Sets default values for this component's properties
UPDWDeployableComponent::UPDWDeployableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPDWDeployableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPDWDeployableComponent::ResetActor()
{
	Super::ResetActor();
	if (IsValid(InteractionVFXComponent))
	{
		InteractionVFXComponent->DestroyComponent();
	}
	GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GetOwner()->SetActorTransform(InitialTransform);
	UPDWInteractionReceiverComponent* IntComp = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
	if (IntComp)
	{
		IntComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		IntComp->AddStateTag(InitialState);
	}
}

void UPDWDeployableComponent::OnMinigameEnded()
{
	if (IsValid(InteractionVFXComponent))
	{
		InteractionVFXComponent->DestroyComponent();
	}
	Super::OnMinigameEnded();
}

void UPDWDeployableComponent::OnObjectDetached()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttachDetachVFX, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	BP_OnObjectDetached();
}

void UPDWDeployableComponent::OnObjectAttached()
{
	if (IsValid(InteractionVFXComponent))
	{
		InteractionVFXComponent->DestroyComponent();
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttachDetachVFX, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	BP_OnObjectAttached();

}
// #TODO_PDW <Maybe start with interaction receiver comp no collision and turn it on when minigame start> [#daniele.m, 6 October 2025, ]
void UPDWDeployableComponent::OnMinigameStarted()
{
	Super::OnMinigameStarted();
	InteractionVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(InteractionVFX,this,NAME_None,FVector::ZeroVector,GetComponentRotation(),EAttachLocation::SnapToTarget,true);
	InitialTransform = GetOwner()->GetActorTransform();
	UPDWInteractionReceiverComponent* IntComp = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
	if (IntComp)
	{
		InitialState = IntComp->GetCurrentState();
	}
	OnMinigameStartedEvent.Broadcast();

}

