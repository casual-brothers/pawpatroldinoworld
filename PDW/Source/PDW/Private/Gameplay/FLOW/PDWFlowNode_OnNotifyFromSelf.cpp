// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_OnNotifyFromSelf.h"
#include "FlowComponent.h"
#include "FlowSubsystem.h"

FName UPDWFlowNode_OnNotifyFromSelf::INPIN_NotifyContainer;

UPDWFlowNode_OnNotifyFromSelf::UPDWFlowNode_OnNotifyFromSelf(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Condition;
	Category = TEXT("Actor");
#endif
	//InputPins.Add(FFlowPin(TEXT("inNotifyTagContainer")));
	OutputPins = { FFlowPin(TEXT("Success"))};

	INPIN_NotifyContainer = GET_MEMBER_NAME_CHECKED(UPDWFlowNode_OnNotifyFromSelf, NotifyTags);
}

void UPDWFlowNode_OnNotifyFromSelf::ExecuteInput(const FName& PinName)
{
	StartObserving();
}

void UPDWFlowNode_OnNotifyFromSelf::OnLoad_Implementation()
{
	StartObserving();
}


void UPDWFlowNode_OnNotifyFromSelf::OnNotifyFromComponent(UFlowComponent* Component, const FGameplayTag& Tag)
{
	ResolvedNotifyContainer = ResolveNotifyContainer();
	if ((!ResolvedNotifyContainer.IsValid() || ResolvedNotifyContainer.HasTagExact(Tag)))
	{
		TriggerFirstOutput(true);
	}
}

void UPDWFlowNode_OnNotifyFromSelf::StartObserving()
{
	if (UFlowSubsystem* FlowSubsystem = GetFlowSubsystem())
	{
		auto comp = Cast<UFlowComponent>(TryGetRootFlowObjectOwner());
		if (IsValid(comp))
		{
			DelegateHandle = comp->OnNotifyFromSelf.AddUObject(this, &UPDWFlowNode_OnNotifyFromSelf::OnNotifyFromComponent);
		}

		// node might finish work immediately as the effect of ObserveActor()
		// we should terminate iteration in this case
		if (GetActivationState() != EFlowNodeState::Active)
		{
			return;
		}
	}
}

void UPDWFlowNode_OnNotifyFromSelf::Cleanup()
{
	UFlowComponent* comp = Cast<UFlowComponent>(TryGetRootFlowObjectOwner());
	if (IsValid(comp))
	{
		comp->OnNotifyFromSelf.Remove(DelegateHandle);
	}
	Super::Cleanup();
}

FGameplayTagContainer UPDWFlowNode_OnNotifyFromSelf::ResolveNotifyContainer() const
{
	FFlowDataPinResult_GameplayTagContainer NotifyContainerResult = TryResolveDataPinAsGameplayTagContainer(INPIN_NotifyContainer);

	if (NotifyContainerResult.Result == EFlowDataPinResolveResult::FailedMissingPin)
	{
		// Handle lookup of a UFlowNode_Timer that predated DataPins
		NotifyContainerResult.Result = EFlowDataPinResolveResult::Success;
		NotifyContainerResult.Value = NotifyTags;
	}

	check(NotifyContainerResult.Result == EFlowDataPinResolveResult::Success);

	return static_cast<FGameplayTagContainer>(NotifyContainerResult.Value);
}

#if WITH_EDITOR
FString UPDWFlowNode_OnNotifyFromSelf::GetNodeDescription() const
{
	return GetNotifyTagsDescription(NotifyTags);
}

#endif
