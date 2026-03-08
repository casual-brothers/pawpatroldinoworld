// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDW_FlowNode_StartInteraction.h"
#include "FlowSubsystem.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"

UPDW_FlowNode_StartInteraction::UPDW_FlowNode_StartInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
	{
		#if WITH_EDITOR
		Category = TEXT("PDW|Quest");
		NodeDisplayStyle = FlowNodeStyle::Default;
		#endif
	}

void UPDW_FlowNode_StartInteraction::ExecuteInput(const FName& PinName)
{
	UFlowSubsystem* FlowSubsystem = GetFlowSubsystem();
	if (!FlowSubsystem) return;

	bool Found = false;

	for (AActor* Actor : FlowSubsystem->GetFlowActorsByTags(InteractableTag, MatchType, AActor::StaticClass(), true))
	{
		if (!Actor) continue;
		Found = true;
		ProcessInteractionAndContinue(Cast<UPDWInteractionReceiverComponent>(Actor->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass())));
		break;
	}

	if (!Found)
	{
		FlowSubsystem->OnComponentRegistered.AddUniqueDynamic(this, &UPDW_FlowNode_StartInteraction::OnComponentRegistered);
	}
}

void UPDW_FlowNode_StartInteraction::OnComponentRegistered(UFlowComponent* Component)
{
	if(!Component || !Component->GetOwner()) return;
	UFlowSubsystem* FlowSubsystem = GetFlowSubsystem();
	if(!FlowSubsystem) return;

	switch (MatchType)
	{
	case EGameplayContainerMatchType::Any:
		if(!Component->IdentityTags.HasAnyExact(InteractableTag)) return;
		break;
	case EGameplayContainerMatchType::All:
		if(!Component->IdentityTags.HasAllExact(InteractableTag)) return;
		break;
	default:
		break;
	}

	FlowSubsystem->OnComponentRegistered.RemoveDynamic(this, &UPDW_FlowNode_StartInteraction::OnComponentRegistered);
	UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(Component->GetOwner()->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
	if (InteractionComp)
	{
		//Wait next tick, the interaction component is not ready yet
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, InteractionComp]()
			{
				ProcessInteractionAndContinue(InteractionComp);
			});
	}
}

void UPDW_FlowNode_StartInteraction::ProcessInteractionAndContinue(UPDWInteractionReceiverComponent* InteractionComp)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC || !InteractionComp) return;

	FPDWInteractionPayload Payload{};
	Payload.Executioner = PC->GetPawn();
	Payload.InteractionInfo.AddTag(InteractionType);
	InteractionComp->ProcessInteractionForced(Payload);
	TriggerFirstOutput(true);
}
