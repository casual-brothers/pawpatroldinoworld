// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/FLOW/PDWFlowNode_OnInteractionFinish.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "FlowComponent.h"
#include "Managers/QuestSubsystem.h"
#include "FlowSubsystem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"

void UPDWFlowNode_OnInteractionFinish::ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	if(Actor == nullptr || Component == nullptr || ResettingDL) return;

	UPDWInteractionReceiverComponent* InteractionComponent = Cast<UPDWInteractionReceiverComponent>(Actor->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
	if(!InteractionComponent) return; //Experimental. 

	// #DEV <Experimental, if we have issue with interactable con n states we will need to change saving also on interaction succsefful but then we need to find a way to get the prev state before the target one.> [#daniele.m, 14 October 2025, ObserveActor]
	if (UPDWDataFunctionLibrary::GetInteractableState(this, InteractionComponent->GetInteractableID()).IdentityTags.IsValid())
	{
		return;
	}

	Super::ObserveActor(Actor,Component);

	if (InteractionComponent->NeedsReset && DataLayerToReset)
	{
		ResetDl();
	}
}

void UPDWFlowNode_OnInteractionFinish::ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	Super::ForgetActor(Actor,Component);
}

void UPDWFlowNode_OnInteractionFinish::OnInteractionSuccessful(const FPDWInteractionPayload& Payload)
{
	if(!RegisteredActors.Contains(Payload.InteractionReceiverRef->GetOwner())) return;
	if (!NotifyTags.HasAnyExact(Payload.BehaviourExecutedIdentifier.GetSingleTagContainer()) || !Payload.InteractionReceiverRef) return;
	
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestSubsystem->RemoveQuestTarget(Payload.InteractionReceiverRef->GetOwner(), TargetsData);

		//only to save the fact that was already used for the quest
		FInteractableData InteractableData = {};
		InteractableData.State = FGameplayTag::EmptyTag;
		InteractableData.IdentityTags = IdentityTags;
		UPDWDataFunctionLibrary::SetInteractableState(this,Payload.InteractionReceiverRef->GetInteractableID(), InteractableData);
		OnEventReceived();
	}
	
}

void UPDWFlowNode_OnInteractionFinish::OnStateTagAdded(UPDWInteractionReceiverComponent* Component, const FGameplayTag& PrevTag, const FGameplayTag& NewTag)
{
	if(!RegisteredActors.Contains(Component->GetOwner())) return;
	if (!Component || !NotifyTags.HasAnyExact(NewTag.GetSingleTagContainer())) return;

	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		//i'm saving the previous state so i can trigger it to set up back the state and all the animation etc
		QuestSubsystem->RemoveQuestTarget(Component->GetOwner(), TargetsData);
		FInteractableData InteractableData = {};
		InteractableData.State = PrevTag;
		InteractableData.IdentityTags = IdentityTags;
		UPDWDataFunctionLibrary::SetInteractableState(this, Component->GetInteractableID(), InteractableData);
		OnEventReceived();
	}
}

void UPDWFlowNode_OnInteractionFinish::TriggerOutput(FName PinName, const bool bFinish /*= false*/, const EFlowPinActivationType ActivationType /*= EFlowPinActivationType::Default*/)
{
	if (bFinish)
	{
		if (!KeepSavedState)
		{
			UPDWDataFunctionLibrary::RemoveInteractableStateByIdentityTags(this, IdentityTags);
		}

		UnbindEvents();
	}

	Super::TriggerOutput(PinName, bFinish, ActivationType);
}

void UPDWFlowNode_OnInteractionFinish::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (PinName == TEXT("Start"))
	{
		BindEvents();
	}
}

void UPDWFlowNode_OnInteractionFinish::BindEvents()
{
	if (NotifyType == ENotifyType::StateChanged)
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnQuestInteractionStateAdd.AddUniqueDynamic(this, &UPDWFlowNode_OnInteractionFinish::OnStateTagAdded);
		}
	}
	else if (NotifyType == ENotifyType::BehaviourSuccess)
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnQuestInteractionSuccess.AddUniqueDynamic(this, &UPDWFlowNode_OnInteractionFinish::OnInteractionSuccessful);
		}
	}	
}

void UPDWFlowNode_OnInteractionFinish::UnbindEvents()
{
	if (NotifyType == ENotifyType::StateChanged)
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnQuestInteractionStateAdd.RemoveDynamic(this, &UPDWFlowNode_OnInteractionFinish::OnStateTagAdded);
		}
	}
	else if (NotifyType == ENotifyType::BehaviourSuccess)
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnQuestInteractionSuccess.RemoveDynamic(this, &UPDWFlowNode_OnInteractionFinish::OnInteractionSuccessful);
		}
	}
}

void UPDWFlowNode_OnInteractionFinish::ResetDl()
{
	ResettingDL = true;
	TMap<TObjectPtr<UDataLayerAsset>, bool> UnloadDL = {{DataLayerToReset, false}};
	RegisteredActors.Empty();
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), UnloadDL);

		GetWorld()->GetTimerManager().SetTimer(ResetDLTimerHandler, [=, this]()
			{		
				if (UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(GetWorld()))
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Check DL")));
					const UDataLayerInstance* DLInstance = DataLayerManager->GetDataLayerInstanceFromAsset(DataLayerToReset);
					if (DLInstance || (DLInstance && DLInstance->GetRuntimeState() == EDataLayerRuntimeState::Unloaded))
					{
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Reload DL!")));
						GEngine->ForceGarbageCollection(true);
						TMap<TObjectPtr<UDataLayerAsset>, bool> LoadDL = { {DataLayerToReset, true} };
						UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), LoadDL);
						GetWorld()->GetTimerManager().ClearTimer(ResetDLTimerHandler);
						ResettingDL = false;
					}
				}
			},
		0.1f,
		true);
}

void UPDWFlowNode_OnInteractionFinish::OnLoad_Implementation()
{
	BindEvents();
	Super::OnLoad_Implementation();
}

void UPDWFlowNode_OnInteractionFinish::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		UnbindEvents();
		UPDWDataFunctionLibrary::RemoveInteractableStateByIdentityTags(this, IdentityTags);
	}
	
	Super::OnPassThrough_Implementation();
}
