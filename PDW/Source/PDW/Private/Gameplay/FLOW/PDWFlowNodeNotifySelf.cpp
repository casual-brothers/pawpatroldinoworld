// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNodeNotifySelf.h"
#include "FlowComponent.h"
#include "FlowSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDWFlowNodeNotifySelf)

UPDWFlowNodeNotifySelf::UPDWFlowNodeNotifySelf(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NetMode(EFlowNetMode::Authority)
{
#if WITH_EDITOR
	Category = TEXT("PDW");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNodeNotifySelf::ExecuteInput(const FName& PinName)
{
	auto comp = Cast<UFlowComponent>(TryGetRootFlowObjectOwner());
	if (IsValid(comp))
		comp->NotifyFromOwnerGraph(NotifyTags, NetMode);
	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FString UPDWFlowNodeNotifySelf::GetNodeDescription() const
{
	return GetNotifyTagsDescription(NotifyTags);
}

EDataValidationResult UPDWFlowNodeNotifySelf::ValidateNode()
{
	return EDataValidationResult::Valid;
}
#endif