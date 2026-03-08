// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "UI/HUD/PDWInteractionComponentWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Data/GameplayTagsDefinitions.h"


#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayTagsFunctionLibrary.h"
#include "Gameplay/Interaction/PDWOverlapBehaviour.h"
#include "Data/PDWPlayerState.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/MiniGames/ConditionsCheck/PDWConditionCheck.h"
#include "Gameplay/Components/PDWTagComponent.h"
#include "QuestSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Components/PDWVehicleInteractionComponent.h"

DECLARE_CYCLE_STAT(TEXT("InteractionReceiverComp ~ InitInteraction"), STAT_InitInteraction, STATGROUP_InteractionReceiver);
DECLARE_CYCLE_STAT(TEXT("InteractionReceiverComp ~ ExecuteUIOverlapBehaviour"), STAT_ExecuteUIOverlapBehaviour, STATGROUP_InteractionReceiver);
DECLARE_CYCLE_STAT(TEXT("InteractionReceiverComp ~ EvaluateBehaviourCondition"), STAT_EvaluateBehaviourCondition, STATGROUP_InteractionReceiver);
DECLARE_CYCLE_STAT(TEXT("InteractionReceiverComp ~ GetHighestPriorityTagSupported"), STAT_GetHighestPriorityTagSupported, STATGROUP_InteractionReceiver);
DECLARE_CYCLE_STAT(TEXT("InteractionReceiverComp ~ ProcessInteraction"), STAT_ProcessInteraction, STATGROUP_InteractionReceiver);
DECLARE_CYCLE_STAT(TEXT("InteractionReceiverComp ~ ExecuteInteractionBehaviour"), STAT_ExecuteInteractionBehaviour, STATGROUP_InteractionReceiver);

UPDWInteractionReceiverComponent::UPDWInteractionReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetCollisionProfileName(UPDWGameSettings::GetInteractionReceiverProfileName().Name);
	bCanEverAffectNavigation =false;
}

uint8 UPDWInteractionReceiverComponent::GetInteractionConfiguration()
{
	if (DataConfigurationInstance)
	{
		return DataConfigurationInstance->InteractionConfiguration;
	}
	#if WITH_EDITOR
	auto Text = "Missing \'{0}\'";
    FText MissingElement = FText::FromString(GetOwner()->GetName());
    FText Title = FText::FromString(TEXT("PLEASE IMPLEMENT ME"));
	 FText Content = FText::Format(FText::FromString(Text), MissingElement);
    FMessageDialog::Open(EAppMsgType::Ok, Content,Title);
	#endif //WITH_EDITOR
	return 0;
}

void UPDWInteractionReceiverComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPDWInteractionReceiverComponent::BeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPDWInteractionReceiverComponent::EndOverlap);

	ensureMsgf(DataConfiguration,TEXT("InteractionReceiverComponent : MISSING CONFIGURATION DATA - CAN'T PLAY"));
	if (!DataConfiguration)
	{
		return;
	}
	DataConfigurationInstance = NewObject<UPDWInteractionReceiverConfigData>(this, DataConfiguration);
	InitInteractions();
	TagComp = GetOwner()->FindComponentByClass<UPDWTagComponent>();
	if (TagComp)
	{
		TagComp->OnPDWTagAdded.AddUniqueDynamic(this,&UPDWInteractionReceiverComponent::OnQuestTagAdded);
	}

	const FInteractableData Data = UPDWDataFunctionLibrary::GetInteractableState(this,InteractableIDD);
	if(Data.State.IsValid())
	{
		CurrentState = Data.State;
		ForceBehaviourByState();
	}
}

void UPDWInteractionReceiverComponent::ForceBehaviourByState()
{
	if (DataConfigurationInstance && DataConfigurationInstance->InteractionBehaviourMapping.Contains(CurrentState))
	{
		TArray<FGameplayTag> Keys;
		DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.GetKeys(Keys);
		if (Keys.Num())
		{
			if (DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(Keys[0]))
			{
				ExecuteInteractionBehaviour(DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[Keys[0]].BehaviourIdentifier,GetOwner(), false, true);
			}
		}
	}
}

void UPDWInteractionReceiverComponent::InitInteractions()
{
	SCOPE_CYCLE_COUNTER(STAT_InitInteraction);
	if (DataConfigurationInstance)
	{
		CurrentState = DataConfigurationInstance->StartingState;
		DefaultStateTag = DataConfigurationInstance->StartingState;
	}
	TMap<FGameplayTag, TObjectPtr<UPDWInteractionBehaviour>> InteractionsToAdd = DataConfigurationInstance->Behaviours;
	for (auto& Interaction : DataConfigurationInstance->Behaviours)
	{
		if (IsValid(DataConfigurationInstance->Behaviours[Interaction.Key]))
			DataConfigurationInstance->Behaviours[Interaction.Key]->InitBehaviour(this, Interaction.Key);
	}
}

void UPDWInteractionReceiverComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(InteractionInProgress))
	{
		InteractionInProgress->TickBehaviour(DeltaTime);
	}
	//if (IsValid(OverlapBehaviourInProgress))
	//{
	//	OverlapBehaviourInProgress->TickBehaviour(DeltaTime);
	//}

}

void UPDWInteractionReceiverComponent::ExecuteUIOverlapBehaviour(const FGameplayTag& inBehaviourTagToTrigger, UPDWInteractionComponent* Sender)
{
	SCOPE_CYCLE_COUNTER(STAT_ExecuteUIOverlapBehaviour);
	if (DataConfigurationInstance->OverlapBehaviours.Contains(inBehaviourTagToTrigger))
	{
		if (OverlapBehavioursInProgress.Contains(Sender))
		{
			return;
		}

		UPDWOverlapBehaviour* OverlapBehaviour = DataConfigurationInstance->OverlapBehaviours[inBehaviourTagToTrigger];
		FPDWOverlapBehaviourInizializzation Inizialization;
		Inizialization.Receiver = this;
		Inizialization.Sender = Sender;
		if (OverlapBehavioursInProgress.Num())
		{
			//is player 2 so i need to instanciate a new overlap Behaviour;
			OverlapBehaviour = NewObject<UPDWOverlapBehaviour>(this, DataConfigurationInstance->OverlapBehaviours[inBehaviourTagToTrigger]->GetClass());
		}
		OverlapBehaviour->InitBehaviour(Inizialization);
		OverlapBehaviour->ExecuteBehaviour();
		OverlapBehavioursInProgress.Add(Sender,OverlapBehaviour);
	}
}

void UPDWInteractionReceiverComponent::EndUIOverlapBehaviour(UPDWInteractionComponent* Sender)
{
	if (OverlapBehavioursInProgress.Contains(Sender))
	{
		if (OverlapBehavioursInProgress[Sender])
		{
			OverlapBehavioursInProgress[Sender]->EndBehaviour();
			OverlapBehavioursInProgress.Remove(Sender);
		}
	}
}

void UPDWInteractionReceiverComponent::OnInputReceivedInteractionSuccess()
{
	for (TPair<FGameplayTag,TObjectPtr<UPDWOverlapBehaviour>> Behaviour : DataConfigurationInstance->OverlapBehaviours)
	{
		Behaviour.Value->OnInteractionSuccessfull();
	}
}

void UPDWInteractionReceiverComponent::NotifyInteractionResultToUIOverlapBehaviour(const bool bSuccess, AActor* inActor)
{
	//if (OverlapBehaviourInProgress)
	//{
	//	if (bSuccess)
	//	{
	//		OverlapBehaviourInProgress->OnInteractionSuccessfull();
	//	}
	//	else
	//	{
	//		OverlapBehaviourInProgress->OnInteractionFailed();
	//	}
	//}
}

bool UPDWInteractionReceiverComponent::EvaluateBehaviourCondition(const FGameplayTag& inBehaviourTag,const FPDWInteractionPayload& inPayload)
{
	SCOPE_CYCLE_COUNTER(STAT_EvaluateBehaviourCondition);
	//IF AT LEAST 1 CONDITION FAIL, ALL FAIL.
	FPDWInteractReceiverPayload ReceiverPayload;
	ReceiverPayload.ReceiverActor = GetOwner();
	ReceiverPayload.ReceiverStates = CurrentState;

	for (UPDWConditionCheck* Condition : DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[inBehaviourTag].BehaviourConditions)
	{
			if (!Condition->EvaluateCondition(inPayload,ReceiverPayload))
		{
			return false;
		}
	}
	return true;
}

void UPDWInteractionReceiverComponent::OnVehicleComponentDestroyed(UActorComponent* inComponent)
{
	inComponent->OnComponentDeactivated.RemoveDynamic(this,&UPDWInteractionReceiverComponent::OnVehicleComponentDestroyed);
	//Clean Map of overlap behaviours
	if (UPDWInteractionComponent* InteractionComp = Cast<UPDWInteractionComponent>(inComponent))
	{
		UPDWInteractionComponent* KeyToRemove = nullptr;
		for (const auto [InteractionComponent, OverlapBehaviour] : OverlapBehavioursInProgress)
		{
			if (InteractionComponent == InteractionComp)
			{
				KeyToRemove = InteractionComponent;
			}
		}
		if (KeyToRemove)
		{
			EndUIOverlapBehaviour(KeyToRemove);
		}
	}
}

void UPDWInteractionReceiverComponent::BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	if (UPDWInteractionComponent* InteractionComp = Cast<UPDWInteractionComponent>(InOtherComp))
	{
		if (UPDWVehicleInteractionComponent* VehicleInteractionComp = Cast<UPDWVehicleInteractionComponent>(InOtherComp))
		{
			VehicleInteractionComp->OnComponentDeactivated.AddUniqueDynamic(this,&UPDWInteractionReceiverComponent::OnVehicleComponentDestroyed);
		}
		if(DataConfigurationInstance->OverlapBehaviours.Num() == 0)
			return;
		TArray<FGameplayTag> Keys;
		DataConfigurationInstance->OverlapBehaviours.GetKeys(Keys);
		if (Keys.IsValidIndex(0))
		{
			ExecuteUIOverlapBehaviour(Keys[0],InteractionComp);
		}
	}
}

void UPDWInteractionReceiverComponent::EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex)
{
	if (UPDWInteractionComponent* InteractionComp = Cast<UPDWInteractionComponent>(InOtherComp))
	{
		TArray<FGameplayTag> Keys;
		DataConfigurationInstance->OverlapBehaviours.GetKeys(Keys);
		EndUIOverlapBehaviour(InteractionComp);
	}
}

void UPDWInteractionReceiverComponent::OnQuestTagAdded(const FGameplayTagContainer& UpdatedContainer, const FGameplayTag& NewTagAdded)
{
	if (NewTagAdded.MatchesTag(UQuestSettings::GetQuestTargetTag()))
	{
		if (CurrentState != DefaultStateTag)
		{
			const FInteractableData Data = UPDWDataFunctionLibrary::GetInteractableState(this,InteractableIDD);
			if(!Data.State.IsValid())
			{
				if (UseDLReset)
				{
					NeedsReset = true;
				}
				else
				{
					//if it's saved emans qwe don't want to reset it
					//Set the state to reset and trigger the reset behaviour
					CurrentState = ResetStateTag;
					FPDWInteractionPayload Payload;
					Payload.InteractionInfo = FGameplayTag::RequestGameplayTag("InteractionBehaviour.Reset").GetSingleTagContainer();
					ProcessInteractionForced(Payload);
				}
			}
		}
	}
}

const FGameplayTag UPDWInteractionReceiverComponent::GetHighestPriorityTagSupported(const FPDWInteractionPayload& inPayload)
{
	SCOPE_CYCLE_COUNTER(STAT_GetHighestPriorityTagSupported);
	TArray<FGameplayTag> InteractionBehaviourMappingTagArray;

	DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.GetKeys(InteractionBehaviourMappingTagArray);
	int32 CurrentBehaviorPriority = INT_MAX;
	FGameplayTag FoundTag = FGameplayTag::EmptyTag;
	//#TODO REWRITE AND CACHE SOMETHING, TO MAKE EASY TO READ WHAT'S GOING ON
	if (InteractionBehaviourMappingTagArray.Num())
	{
		for (int i = 0; i < InteractionBehaviourMappingTagArray.Num(); i++)
		{
			if (inPayload.InteractionType.MatchesTag(InteractionBehaviourMappingTagArray[i]))
			{
				if (DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[InteractionBehaviourMappingTagArray[i]].BehaviorPriority < CurrentBehaviorPriority)
				{
					CurrentBehaviorPriority = DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[InteractionBehaviourMappingTagArray[i]].BehaviorPriority;
					FoundTag = InteractionBehaviourMappingTagArray[i];
				}
			}
			if (inPayload.InteractionInfo.HasTag(InteractionBehaviourMappingTagArray[i]))
			{
				if (DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[InteractionBehaviourMappingTagArray[i]].BehaviorPriority < CurrentBehaviorPriority)
				{
					CurrentBehaviorPriority = DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[InteractionBehaviourMappingTagArray[i]].BehaviorPriority;
					FoundTag = InteractionBehaviourMappingTagArray[i];
				}
			}
		}
	}
	return FoundTag;
}

void UPDWInteractionReceiverComponent::AddStateTag(const FGameplayTag& Tag)
{
	if (CurrentState != Tag)
	{
		OnStatusTagAdded.Broadcast(this, CurrentState, Tag);
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnInteractionStateAddEvent(this, CurrentState, Tag);
		}
		CurrentState = Tag;
		//PROBABLY SHOULD BE REMOVED
	}
}

void UPDWInteractionReceiverComponent::RemoveStateTag(const FGameplayTag& Tag)
{
	if (CurrentState == Tag)
	{
		CurrentState = FGameplayTag::EmptyTag;
		OnStatusTagRemoved.Broadcast(this, Tag, CurrentState);
	}
}

FGameplayTag UPDWInteractionReceiverComponent::GetCurrentState() const
{
	return CurrentState;
}

FGameplayTagContainer UPDWInteractionReceiverComponent::GetAllowedInteractionInCurrentState()
{
	FGameplayTagContainer StateAllowedSkills;
	if (DataConfigurationInstance)
	{
		if (DataConfigurationInstance->InteractionBehaviourMapping.Contains(CurrentState))
		{
			for (const auto& [Tag, Config] : DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping)
			{
				StateAllowedSkills.AddTag(Tag);
			}
		}
	}
	return StateAllowedSkills;
}

FGameplayTagContainer UPDWInteractionReceiverComponent::GetAllowedInteractionBasedOnState(const FGameplayTag& inStateTag)
{
	FGameplayTagContainer StateAllowedSkills;
	if (DataConfigurationInstance->InteractionBehaviourMapping.Contains(inStateTag))
	{
		for (const auto& [Tag, Config] : DataConfigurationInstance->InteractionBehaviourMapping[inStateTag].StateBehaviourMapping)
		{
			StateAllowedSkills.AddTag(Tag);
		}
	}
	return StateAllowedSkills;
}

void UPDWInteractionReceiverComponent::RemoveBehaviour(const FGameplayTag& BehaviourIdentifier)
{
	if (DataConfigurationInstance->Behaviours.Contains(BehaviourIdentifier))
	{
		DataConfigurationInstance->Behaviours.Remove(BehaviourIdentifier);
	}
}

void UPDWInteractionReceiverComponent::RemoveInteracter(UPDWInteractionComponent* Interacted)
{
	//EndUIOverlapBehaviour(Interacted);
	InteractionComponents.Remove(Interacted);
	OnStopBeingInteractable.Broadcast(Interacted);
}

bool UPDWInteractionReceiverComponent::ProcessInteraction(const FPDWInteractionPayload& Payload,bool& bIsAutoSwap)
{
	SCOPE_CYCLE_COUNTER(STAT_ProcessInteraction);
	if(!DataConfigurationInstance || !DataConfigurationInstance->InteractionBehaviourMapping.Contains(CurrentState))
		return false;
	OnInteract.Broadcast(Payload);
	FGameplayTag FoundTag = GetHighestPriorityTagSupported(Payload);
	if (!DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(FoundTag) && !DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(Payload.InteractionType))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("DataConfigurationInstance->InteractionBehaviourMapping doesn't contains %s from %s"), *FoundTag.ToString(), *Payload.InteractionType.ToString()));
		OnInteractionFailed.Broadcast(Payload);
		NotifyInteractionResultToUIOverlapBehaviour(false,Payload.Executioner);
		return false;
	}
	
	//Once we got the best tag for interaction we need to evaluate conditions to check if it's executable or not
	if (DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(FoundTag))
	{
		if (EvaluateBehaviourCondition(FoundTag,Payload))
		{
			Payload.BehaviourExecutedIdentifier = DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[FoundTag].BehaviourIdentifier;
			NotifyInteractionResultToUIOverlapBehaviour(true,Payload.Executioner);
			ExecuteInteractionBehaviour(DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[FoundTag].BehaviourIdentifier, Payload.Executioner);
			Payload.InteractionReceiverState.AddTag(CurrentState);
			Payload.InteractionReceiverRef = this;
			OnInteractionSuccessful.Broadcast(Payload);
			if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
			{
				EventSubsystem->OnInteractionSuccessEvent(Payload);
			}
		}
		else
		{
			ExecuteInteractionBehaviour(DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[FoundTag].FailedConditionBehaviour, Payload.Executioner);
			Payload.InteractionReceiverState.AddTag(CurrentState);
			Payload.BehaviourExecutedIdentifier = DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[FoundTag].FailedConditionBehaviour;
		}
	}
	return true;
}

bool UPDWInteractionReceiverComponent::ProcessInteractionForced(const FPDWInteractionPayload& Payload)
{
	if (!DataConfigurationInstance->InteractionBehaviourMapping.Contains(CurrentState))
	{
		return false;
	}
	OnInteract.Broadcast(Payload);
	FGameplayTag FoundTag = GetHighestPriorityTagSupported(Payload);

	if (!DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(FoundTag) && !DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(Payload.InteractionType))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("DataConfigurationInstance->InteractionBehaviourMapping doesn't contains %s from %s"), *FoundTag.ToString(), *Payload.InteractionType.ToString()));
		OnInteractionFailed.Broadcast(Payload);
		NotifyInteractionResultToUIOverlapBehaviour(false,Payload.Executioner);
		return false;
	}
	
	if (DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping.Contains(FoundTag))
	{
		Payload.BehaviourExecutedIdentifier = DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[FoundTag].BehaviourIdentifier;
		ExecuteInteractionBehaviour(DataConfigurationInstance->InteractionBehaviourMapping[CurrentState].StateBehaviourMapping[FoundTag].BehaviourIdentifier, Payload.Executioner,false,true);
		NotifyInteractionResultToUIOverlapBehaviour(true,Payload.Executioner);
		Payload.InteractionReceiverState.AddTag(CurrentState);
		Payload.InteractionReceiverRef = this;
		OnInteractionSuccessful.Broadcast(Payload);
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnInteractionSuccessEvent(Payload);
		}
	}
	return true;
}

void UPDWInteractionReceiverComponent::AddInteracter(UPDWInteractionComponent* Interacter,const FGameplayTagContainer& SupportedInteractions)
{
	InteractionComponents.Emplace(Interacter);
	const FGameplayTagContainer FitleredContainer = SupportedInteractions.Filter(DataConfigurationInstance->AllowedInteractions);
	for (const FGameplayTag& Tag : FitleredContainer)
	{
		const FGameplayTag RootTag = UPDWGameplayTagsFunctionLibrary::GetRootTagFromTag(Tag);
		ExecuteUIOverlapBehaviour(RootTag, Interacter);
	}
	OnBecomeInteractable.Broadcast(Interacter);
}

void UPDWInteractionReceiverComponent::AddInteractionBehaviour(const FGameplayTag& BehaviourIdentifier, UPDWInteractionBehaviour* Behaviour)
{
	if (!DataConfigurationInstance->Behaviours.Contains(BehaviourIdentifier))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Interaction already present")));
		return;
	}
	if (!IsValid(Behaviour))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Invalid interaction")));
		return;
	}
	DataConfigurationInstance->Behaviours.Add(BehaviourIdentifier, Behaviour);
	DataConfigurationInstance->Behaviours[BehaviourIdentifier]->InitBehaviour(this, BehaviourIdentifier);
}

void UPDWInteractionReceiverComponent::ExecuteInteractionBehaviour(const FGameplayTag& BehaviourIdentifier, AActor* Executioner /*= nullptr*/, bool RemoveBehaviourWhenFinished /*= false*/,bool bSkipSteps /*=false*/)
{
	SCOPE_CYCLE_COUNTER(STAT_ExecuteInteractionBehaviour);
	if (!DataConfigurationInstance->Behaviours.Contains(BehaviourIdentifier) || !IsValid(DataConfigurationInstance->Behaviours[BehaviourIdentifier]))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Interaction not found")));
		return;
	}
	if (IsValid(InteractionInProgress))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Starting new interaction behaviour when the previous isn't ended")));
		return;
	}
	SetCurrentBehaviour(DataConfigurationInstance->Behaviours[BehaviourIdentifier]);
	if (bSkipSteps)
	{
		DataConfigurationInstance->Behaviours[BehaviourIdentifier]->SetSkipSteps(bSkipSteps);
	}
	DataConfigurationInstance->Behaviours[BehaviourIdentifier]->ExecuteBehaviour(Executioner, RemoveBehaviourWhenFinished);
}

void UPDWInteractionReceiverComponent::SetCurrentBehaviour(UPDWInteractionBehaviour* NewBehaviourInProgress)
{
	InteractionInProgress = NewBehaviourInProgress;
	if (IsValid(NewBehaviourInProgress))
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}

void UPDWInteractionReceiverComponent::BehaviourFinished(UPDWInteractionBehaviour* NewBehaviourInProgress)
{
	SetCurrentBehaviour(nullptr);
	OnBehaviourFinished.Broadcast(NewBehaviourInProgress);
	if (bResetCollisionAtEndBehaviour)
	{
		ResetCollisions();
	}
}

void UPDWInteractionReceiverComponent::PostEditImport()
{
	Super::PostEditImport();
	InteractableIDD = -1;
}

void UPDWInteractionReceiverComponent::ResetCollisions()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

