// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_OnNotifyFromMinigame.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Managers/QuestSubsystem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

UPDWFlowNode_OnNotifyFromMinigame::UPDWFlowNode_OnNotifyFromMinigame(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	#if WITH_EDITOR
	Category = TEXT("PDW|Quest");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif

	OutputPins.Add(FFlowPin(TEXT("Exit")));
}

void UPDWFlowNode_OnNotifyFromMinigame::OnEventReceived()
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();	
	if(!EventSubsystem)
		return;

	EventSubsystem->OnSwitcherStateWaitForAction.AddUniqueDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::ContinueToNextNode);
}

void UPDWFlowNode_OnNotifyFromMinigame::ContinueToNextNode()
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();	
	if(!EventSubsystem)
		return;

	EventSubsystem->OnSwitcherStateWaitForAction.RemoveDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::ContinueToNextNode);

	Super::OnEventReceived();
}

void UPDWFlowNode_OnNotifyFromMinigame::ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	Super::ObserveActor(Actor, Component);

	if (RegisteredActors.Contains(Actor))
	{
		if (UPDWMinigameConfigComponent* MinigameComponent = Cast<UPDWMinigameConfigComponent>(Actor->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass())))
		{
			MinigameComponent->OnMinigameInitializationComplete.AddUniqueDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::OnMinigameInitComplete);
			MinigameComponent->OnMinigameComplete.AddUniqueDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::OnMinigameComplete);
			MinigameComponent->OnMinigameLeft.AddUniqueDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::OnMinigameLeft);
		}
	}
}

void UPDWFlowNode_OnNotifyFromMinigame::ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	Super::ForgetActor(Actor, Component);

	if (RegisteredActors.Contains(Actor))
	{
		if (UPDWMinigameConfigComponent* MinigameComponent = Cast<UPDWMinigameConfigComponent>(Actor->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass())))
		{
			MinigameComponent->OnMinigameInitializationComplete.RemoveDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::OnMinigameInitComplete);
			MinigameComponent->OnMinigameComplete.RemoveDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::OnMinigameComplete);
			MinigameComponent->OnMinigameLeft.RemoveDynamic(this, &UPDWFlowNode_OnNotifyFromMinigame::OnMinigameLeft);
		}
	}
}

void UPDWFlowNode_OnNotifyFromMinigame::OnMinigameComplete(const FMiniGameEventSignature& inSignature)
{	
	UpdateDataLayer(MinigameLeftOrCompletedLayersInfo);

	if (inSignature.MiniGameComponent)
	{
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			QuestSubsystem->RemoveQuestTarget(inSignature.MiniGameComponent->GetOwner(), TargetsData);		
		}

		UPDWMinigameConfigData* MinigameConfigData = inSignature.MiniGameComponent->GetMinigameConfigurationData();
		if (MinigameConfigData && MinigameConfigData->bIsReplayable)
		{
			FPDWMinigameInfo MinigameInfo;
			MinigameInfo.IdentityTags = IdentityTags;
			MinigameInfo.Area = UPDWDataFunctionLibrary::GetPlayerAreaId(this);
			UPDWDataFunctionLibrary::AddPlayedMinigame(this, MinigameInfo);
		}
	}

	OnEventReceived();	
}

void UPDWFlowNode_OnNotifyFromMinigame::OnMinigameLeft(const FMiniGameEventSignature& inSignature)
{
	UpdateDataLayer(MinigameLeftOrCompletedLayersInfo);
	TriggerOutput("Exit");
}

void UPDWFlowNode_OnNotifyFromMinigame::OnMinigameInitComplete(const TArray<APDWPlayerController*>& Controllers)
{
	UpdateDataLayer(MinigameInitCompleteLayersInfo);
}
