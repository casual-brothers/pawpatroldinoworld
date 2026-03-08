#include "Components/NebulaInteractionReceiverComponent.h"
#include "NebulaInteractionSystemSettings.h"
#include "Components/InteractionFlowComponent.h"

UNebulaInteractionReceiverComponent::UNebulaInteractionReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetCollisionProfileName(UNebulaInteractionSystemSettings::GetInteractionReceiverCollisionProfileName().Name);
	if (bBindToInputAction)
	{
		InteractionActionTag = UNebulaInteractionSystemSettings::GetInteractionActionTag();
	}
}


#pragma region CORE

bool UNebulaInteractionReceiverComponent::ProcessInteraction(FInteractionInfo& inPayload)
{
	inPayload.Interactable = GetOwner();
	//Add if needed
	/*inPayload.InteractionReceiverTags = Something;*/
	if (!FlowComponent)
	{
		FlowComponent = GetOwner()->FindComponentByClass<UInteractionFlowComponent>();
	}
	ensureMsgf(FlowComponent, TEXT("NebulaInteractionReceiverComponent::ProcessInteraction - FlowComponent is not set. Please ensure that the owner has a UInteractionFlowComponent."));
	FInstancedStruct Struct = FInstancedStruct::Make<FInteractionInfo>(inPayload);
	FlowComponent->TriggerInteraction(Struct);
	return true;
}
void UNebulaInteractionReceiverComponent::BeginPlay()
{
	Super::BeginPlay();
	FlowComponent = GetOwner()->FindComponentByClass<UInteractionFlowComponent>();
}
#pragma endregion

#pragma region SETTERS

#pragma endregion

#pragma region GETTERS

FGameplayTag UNebulaInteractionReceiverComponent::GetInteractionActionTag() const
{
	return InteractionActionTag;
}

#pragma endregion

#pragma region UTILITY
#if WITH_EDITOR
void UNebulaInteractionReceiverComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (bBindToInputAction)
	{
		InteractionActionTag = UNebulaInteractionSystemSettings::GetInteractionActionTag();
	}
}
#endif
#pragma endregion