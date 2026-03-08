// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/WaitForInteractionNode.h"
#include "FlowComponent.h"
#include "Components/InteractionFlowComponent.h"

UWaitForInteractionNode::UWaitForInteractionNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Interaction");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
	OutputPins.Empty();
	//Add default exit if no state are matched
}
#if WITH_EDITOR
void UWaitForInteractionNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnReconstructionRequested.ExecuteIfBound();
}
bool UWaitForInteractionNode::SupportsContextPins() const
{
	return true;
}

TArray<FFlowPin> UWaitForInteractionNode::GetContextOutputs() const
{
	TArray<FFlowPin> ContextOutputs = Super::GetContextOutputs();
	for (const FGameplayTag& Tag : States)
	{
		ContextOutputs.AddUnique(FFlowPin(Tag.GetTagName()));
	}
	return ContextOutputs;
}
#endif

void UWaitForInteractionNode::ExecuteInput(const FName& PinName)
{
	WaitForEvent();
}

void UWaitForInteractionNode::WaitForEvent()
{
	if (UFlowSubsystem* FlowSubsystem = GetFlowSubsystem())
	{
		UInteractionFlowComponent* FlowComp = Cast<UInteractionFlowComponent>(TryGetRootFlowObjectOwner());
		if (IsValid(FlowComp))
		{
			FlowComp->OnInteractionTriggered.AddUObject(this, &UWaitForInteractionNode::OnEventReceived);
		}

		if (GetActivationState() != EFlowNodeState::Active)
		{
			return;
		}
	}
}

void UWaitForInteractionNode::OnEventReceived(UInteractionFlowComponent* inComponent, const FInstancedStruct& inInteractionInfo)
{
	if (IsValid(inComponent))
	{
		OutInteractionInfo.Value = inInteractionInfo;
		TriggerOutput(inComponent->GetInteractionState().GetTagName());
	}
}

void UWaitForInteractionNode::Cleanup()
{
	UInteractionFlowComponent* FlowComp = Cast<UInteractionFlowComponent>(TryGetRootFlowObjectOwner());
	if (IsValid(FlowComp))
	{
		FlowComp->OnInteractionTriggered.RemoveAll(this);
	}
	Super::Cleanup();
}

