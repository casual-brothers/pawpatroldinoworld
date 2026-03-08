// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_OnNotifyFromActor.h"
#include "Managers/QuestSubsystem.h"
#include "FlowComponent.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

UPDWFlowNode_OnNotifyFromActor::UPDWFlowNode_OnNotifyFromActor(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
: Super(ObjectInitializer)
{
	InputPins.Add(FFlowPin(TEXT("TargetTagOverride"), EFlowPinType::GameplayTagContainer));
}

void UPDWFlowNode_OnNotifyFromActor::ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	Super::ObserveActor(Actor,Component);

	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnObservedActorLoadedEvent(Actor.Get(), Component.Get(), TargetsData, this);
	}
		
}

void UPDWFlowNode_OnNotifyFromActor::ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnObservedActorUnloadedEvent(Actor.Get(), Component.Get(), TargetsData, this);
	}

	Super::ForgetActor(Actor,Component);
}

void UPDWFlowNode_OnNotifyFromActor::ExecuteInput(const FName& PinName)
{
	if (PinName == TEXT("Start"))
	{
		if (InitWithInputPins)
		{
			IdentityTags = UFlowNodeBase::TryResolveDataPinAsGameplayTagContainer("TargetTagOverride").Value;
		}

		UpdateDataLayer(OnEnterDataLayersInfo);
		SetTargets();
	}

	Super::ExecuteInput(PinName);
}

void UPDWFlowNode_OnNotifyFromActor::SetTargets()
{
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		TargetsData.CurrentQuantity = SuccessCount;
		TargetsData.TargetQuantity = SuccessLimit;
		TargetsData.IdentityTag = GetIdentityTags();
		QuestSubsystem->SetQuestTargets(TargetsData);
	}
}

void UPDWFlowNode_OnNotifyFromActor::UpdateDataLayer(TMap<TObjectPtr<UDataLayerAsset>, bool> DLInfo)
{
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), DLInfo);
}

void UPDWFlowNode_OnNotifyFromActor::TriggerOutput(FName PinName, const bool bFinish /*= false*/, const EFlowPinActivationType ActivationType /*= EFlowPinActivationType::Default*/)
{
	if (bFinish)
	{
		UpdateDataLayer(OnExitDataLayersInfo);

		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			for (TPair<TWeakObjectPtr<AActor>, TWeakObjectPtr<UFlowComponent>> RegisteredActor : RegisteredActors)
			{
				QuestSubsystem->RemoveQuestTarget(RegisteredActor.Key.Get(), TargetsData);
			}
		}
	}

	Super::TriggerOutput(PinName, bFinish, ActivationType);
}

void UPDWFlowNode_OnNotifyFromActor::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		UpdateDataLayer(OnEnterDataLayersInfo);
		UpdateDataLayer(OnExitDataLayersInfo);
	}

	Super::OnPassThrough_Implementation();
}

void UPDWFlowNode_OnNotifyFromActor::OnLoad_Implementation()
{
	if (InitWithInputPins)
	{
		IdentityTags = UFlowNodeBase::TryResolveDataPinAsGameplayTagContainer("TargetTagOverride").Value;
	}

	Super::OnLoad_Implementation();

	if (SuccessLimit > 0 && SuccessCount == SuccessLimit)
	{
		TriggerOutput(TEXT("Completed"), true);
	}
	else
	{
		SetTargets();
	}
}