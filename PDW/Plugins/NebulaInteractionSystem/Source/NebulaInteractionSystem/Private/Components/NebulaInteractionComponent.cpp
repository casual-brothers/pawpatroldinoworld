#include "Components/NebulaInteractionComponent.h"
#include "Components/NebulaInteractionReceiverComponent.h"
#include "NebulaInteractionSystemSettings.h"


UNebulaInteractionComponent::UNebulaInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetCollisionProfileName(UNebulaInteractionSystemSettings::GetInteractionCollisionProfileName().Name);
}

#pragma region CORE

void UNebulaInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UNebulaInteractionComponent::BeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UNebulaInteractionComponent::EndOverlap);
}

void UNebulaInteractionComponent::EndPlay(EEndPlayReason::Type Reason)
{
	OnComponentBeginOverlap.RemoveDynamic(this, &UNebulaInteractionComponent::BeginOverlap);
	OnComponentEndOverlap.RemoveDynamic(this, &UNebulaInteractionComponent::EndOverlap);
	Super::EndPlay(Reason);
}


bool UNebulaInteractionComponent::TriggerInteraction(UNebulaInteractionReceiverComponent* InteractionReceiverComp)
{

	FInteractionInfo Payload;
	GenerateInteractionPayload(Payload);
	if (IsValid(InteractionReceiverComp))
	{
		return InteractionReceiverComp->ProcessInteraction(Payload);
	}
	return false;
}

void UNebulaInteractionComponent::BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	if (UNebulaInteractionReceiverComponent* InteractionReceiverComp = Cast<UNebulaInteractionReceiverComponent>(InOtherComp))
	{
		if (InteractionConfiguration.bAutomaticInteraction || InteractionReceiverComp->GetInteractionActionTag() == FGameplayTag::EmptyTag)
		{
			TriggerInteraction(InteractionReceiverComp);
		}
		if (InteractionReceiverComp->GetInteractionActionTag().IsValid() && !InteractionConfiguration.bAutomaticInteraction)
		{
			BindInteractionToInput(InteractionReceiverComp->GetInteractionActionTag());
			ReceiverComponentsWaitingForInput.AddUnique(InteractionReceiverComp);
		}
	}
}

void UNebulaInteractionComponent::EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex)
{
	if (UNebulaInteractionReceiverComponent* InteractionReceiverComp = Cast<UNebulaInteractionReceiverComponent>(InOtherComp))
	{
		if (InteractionReceiverComp->GetInteractionActionTag().IsValid() && !InteractionConfiguration.bAutomaticInteraction)
		{
			UnBindInteractionToInput(InteractionReceiverComp->GetInteractionActionTag());
			ReceiverComponentsWaitingForInput.Remove(InteractionReceiverComp);
		}
	}
}

void UNebulaInteractionComponent::GenerateInteractionPayload(FInteractionInfo& outPayload)
{
	outPayload.Interactor = GetOwner();
	outPayload.InteractionTypes = InteractionConfiguration.AllowedInteractions;
	outPayload.InteractionTags = InteractionStatusContainer;
}

void UNebulaInteractionComponent::OnInputReceived(const FInputActionInstance& inInputInstance)
{
	for (int32 i = ReceiverComponentsWaitingForInput.Num() - 1; i >= 0; --i)
	{
		if (ReceiverComponentsWaitingForInput.IsValidIndex(i) || ReceiverComponentsWaitingForInput[i])
		{
			TriggerInteraction(ReceiverComponentsWaitingForInput[i]);
		}
	}
}

void UNebulaInteractionComponent::BindInteractionToInput(FGameplayTag inActionTag /*= FGameplayTag::EmptyTag*/)
{
	//IMPLEMENT YOUR OWN BINDING LOGIC HERE
}

void UNebulaInteractionComponent::UnBindInteractionToInput(FGameplayTag inActionTag /*= FGameplayTag::EmptyTag*/)
{
	//IMPLEMENT YOUR OWN UNBINDING LOGIC HERE
}

#pragma endregion

#pragma region SETTERS

void UNebulaInteractionComponent::AddInteractionStatus(const FGameplayTagContainer& inContainer)
{
	InteractionStatusContainer.AppendTags(inContainer);
}

void UNebulaInteractionComponent::RemoveinteractionStatus(const FGameplayTagContainer& inContainer)
{
	InteractionStatusContainer.RemoveTags(inContainer);
}

void UNebulaInteractionComponent::AddAllowedInteraction(const FGameplayTagContainer& inContainer)
{
	InteractionConfiguration.AllowedInteractions.AppendTags(inContainer);
}

#pragma endregion

#pragma region GETTERS

FGameplayTagContainer UNebulaInteractionComponent::GetInteractionStatus() const
{
	return InteractionStatusContainer;
}

#pragma	endregion