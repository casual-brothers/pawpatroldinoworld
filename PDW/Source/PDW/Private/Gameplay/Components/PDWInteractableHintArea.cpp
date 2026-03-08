// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWInteractableHintArea.h"
#include "Gameplay/Components/PDWTagComponent.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "QuestSettings.h"

UPDWInteractableHintArea::UPDWInteractableHintArea()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UPDWInteractableHintArea::BeginPlay()
{
	Super::BeginPlay();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//if (ConversationToPlay.IsEmpty())
	//{
	//	return;
	//}

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPDWInteractableHintArea::BeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPDWInteractableHintArea::EndOverlap);

	TagComp = GetOwner()->FindComponentByClass<UPDWTagComponent>();
	//if (TagComp)
	//{
	//	TagComp->OnPDWTagAdded.AddUniqueDynamic(this, &UPDWInteractableHintArea::OnQuestTagAdded);
	//	TagComp->OnPDWTagRemoved.AddUniqueDynamic(this, &UPDWInteractableHintArea::OnQuestTagRemoved);
	//}
	QuestTargetTag = UQuestSettings::GetQuestTargetTag();

	InteractionReceiver = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
	if (InteractionReceiver)
	{
		InteractionReceiver->OnStatusTagAdded.AddUniqueDynamic(this,&UPDWInteractableHintArea::OnInteractionStateChange);
		const FGameplayTag CurrentInteractionState = InteractionReceiver->GetCurrentState();
		if (ConversationToPlay.Contains(CurrentInteractionState))
		{
			CurrentState = CurrentInteractionState;
		}
	}

	if(ConversationToPlay.Contains(CurrentState))
		TagComp->AddTag(ConversationToPlay[CurrentState]);
}

void UPDWInteractableHintArea::EndPlay(EEndPlayReason::Type Reason)
{
	//if (TagComp)
	//{
	//	TagComp->OnPDWTagAdded.RemoveDynamic(this, &UPDWInteractableHintArea::OnQuestTagAdded);
	//	TagComp->OnPDWTagRemoved.RemoveDynamic(this, &UPDWInteractableHintArea::OnQuestTagRemoved);
	//}
	Super::EndPlay(Reason);
}

// #DEV <Removed to allow the hint to be fired up even if not in quest> [#daniele.m, 11 December 2025, ]

//void UPDWInteractableHintArea::OnQuestTagAdded(const FGameplayTagContainer& UpdatedContainer, const FGameplayTag& NewTagAdded)
//{
//	if(NewTagAdded == QuestTargetTag)
//	{
//		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
//	}
//}
//
//void UPDWInteractableHintArea::OnQuestTagRemoved(const FGameplayTagContainer& UpdatedContainer, const FGameplayTag& TagRemoved)
//{
//	if(TagRemoved == QuestTargetTag)
//	{
//		SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	}
//}

void UPDWInteractableHintArea::BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	GetWorld()->GetTimerManager().SetTimer(ConversationTriggerDelayHandle, this,&UPDWInteractableHintArea::TriggerConversation,DelayBeforePlay,true);
}

void UPDWInteractableHintArea::EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ConversationTriggerDelayHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ConversationTriggerDelayHandle);
	}
}

void UPDWInteractableHintArea::OnInteractionStateChange(UPDWInteractionReceiverComponent* Component, const FGameplayTag& PrevTag, const FGameplayTag& NewTag)
{
	if(CurrentState != NewTag)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(ConversationTriggerDelayHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(ConversationTriggerDelayHandle);
		}
		if(ConversationToPlay.Contains(CurrentState))
			TagComp->RemoveTag(ConversationToPlay[CurrentState]);
		if(ConversationToPlay.Contains(NewTag))
			TagComp->AddTag(ConversationToPlay[NewTag]);
	}
	CurrentState = NewTag;
}

void UPDWInteractableHintArea::TriggerConversation()
{
	if (ConversationToPlay.Contains(CurrentState) && ConversationPreset.Contains(ConversationToPlay[CurrentState]))
	{
		UPDWDialogueSubSystem::TriggerConversation(this, ConversationPreset[ConversationToPlay[CurrentState]]);
	}
}
