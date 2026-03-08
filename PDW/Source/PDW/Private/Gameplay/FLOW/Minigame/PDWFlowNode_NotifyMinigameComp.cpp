// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Minigame/PDWFlowNode_NotifyMinigameComp.h"
#include "FlowComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"

UPDWFlowNode_NotifyMinigameComp::UPDWFlowNode_NotifyMinigameComp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|NotifyMiniGameComp");
	NodeDisplayStyle = FlowNodeStyle::Logic;
#endif
}

#if WITH_EDITOR
FString UPDWFlowNode_NotifyMinigameComp::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s") , EventTag.IsValid() ? *EventTag.ToString() : TEXT("Invalid Tag"));
}
#endif

void UPDWFlowNode_NotifyMinigameComp::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	UFlowComponent* FlowComp = Cast<UFlowComponent>(TryGetRootFlowObjectOwner());
	UPDWMinigameConfigComponent* MiniGameComp = FlowComp->GetOwner()->FindComponentByClass<UPDWMinigameConfigComponent>();
	MiniGameComp->NotifyEvent(EventTag);
	TriggerFirstOutput(true);
}
