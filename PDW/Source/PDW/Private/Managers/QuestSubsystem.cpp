// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/QuestSubsystem.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Data/PDWLocalPlayer.h"
#include "Data/PDWPersistentUser.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "FlowSubsystem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Components/PDWBreadCrumbsComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Gameplay/FLOW/PDWFlowNode_UpdateQuestStep.h"
#include "QuestSettings.h"
#include "Managers/PDWEventSubsytem.h"
#include "Data/PDWGameplayStructures.h"
#include "UI/WidgetsComponent/PDWIconComponent.h"
#include "Gameplay/Components/PDWBreadcrumbsTargetComponent.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueFlowAsset.h"
#include "Gameplay/Components/PDWTagComponent.h"
#include "Managers/PDWActivitiesManager.h"
#include "FunctionLibraries/PDWActivitiesFunctionLibrary.h"
#include "Gameplay/Actors/PDW_NavigationWaypoint.h"
#include "Modes/PDWGameplayGameMode.h"

UQuestSubsystem* UQuestSubsystem::Get(UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
}

void UQuestSubsystem::QuestStart(UBaseFlowQuest* Quest, bool FromLoad)
{
	ActiveQuests.Emplace(Quest->QuestID, Quest);
	OnQuestStarted.Broadcast(Quest);

	if (!FromLoad)
	{
		UPDWActivitiesManager* Mgr = Cast<UPDWActivitiesManager>(UNebulaFlowActivitiesManager::GetFlowActivitiesManager(this));
		if (Mgr)
		{
			if (!UQuestSettings::GetTutorialQuestTag().MatchesTagExact(Quest->QuestID))
			{
				FString QuestIDSubstring = GetQuestIndexByTag(Quest->QuestID);

				if (QuestIDSubstring == Mgr->FirstTaskId)
				{
					UPDWActivitiesFunctionLibrary::StartMasterActivity(this);
				}
				Mgr->StartActivity(QuestIDSubstring);
			}
		}
	}
}

void UQuestSubsystem::EndQuest(const FGameplayTag& QuestID)
{
	if (ActiveQuests.Contains(QuestID))
	{
		UBaseFlowQuest* Quest = ActiveQuests[QuestID];
		OnQuestEnded.Broadcast(Quest);
		ActiveQuests.Remove(QuestID);
		FQuestData QuestData = {};
		QuestData.QuestID = QuestID;
		QuestData.AreaID =  Quest->GetQuestArea();
		UPDWDataFunctionLibrary::AddCompletedQuest(this,QuestData);

		if (!UQuestSettings::GetTutorialQuestTag().MatchesTagExact(QuestID))
		{
			UPDWActivitiesManager* Mgr = Cast<UPDWActivitiesManager>(UNebulaFlowActivitiesManager::GetFlowActivitiesManager(this));
			if (Mgr)
			{
				FString QuestIDSubstring = GetQuestIndexByTag(Quest->QuestID);
				Mgr->CompleteActivity(QuestIDSubstring);		
			}
		}

		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnQuestCompletedEvent(QuestID);
		}
	}
}

void UQuestSubsystem::StartStep(const FPDWQuestStepData& StepData)
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if (CurrentQuest)
	{
		SetCurrentQuestDescription(StepData.DescriptionData);
		GetCurrentQuest()->SetCurrentQuestStepData(StepData);
		OnQuestStepStartSignature.Broadcast(StepData);
	}
}

UBaseFlowQuest* UQuestSubsystem::GetCurrentQuest()
{
	TArray<UBaseFlowQuest*> CurrentQuests;
	ActiveQuests.GenerateValueArray(CurrentQuests);
	return CurrentQuests.Num() > 0 ? CurrentQuests[0] : nullptr;
}

void UQuestSubsystem::SetQuestTargets(FPDWQuestTargetData TargetData)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC || !QuestConfig || !TargetData.IsMain)
		return;

	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if (!CurrentQuest)
		return;

	//Add fake targets
	if (const UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		if (TargetData.bBreadCrumbsActive)
		{
			TMap<int32, FVector> FakeTargets = {};
			TargetData.FakeTargetsUsed = CurrentQuest->GetCurrentQuestStepData().TargetsData.FakeTargetsUsed;

			for (FPDWQuestTargetLocs& TargetLoc : QuestConfig->TargetLocations)
			{
				//FlowTypes::HasMatchingTags(TargetLoc.TagContainer, TargetData.IdentityTag, TargetLoc.MatchType) ?
				if (TargetLoc.TagContainer.HasAll(TargetData.IdentityTag))
				{
					FakeTargets = TargetLoc.Locations;
					break;
				}
			}

			for (int32 i = 0; i < TargetData.FakeTargetsUsed.Num(); i++)
			{
				FakeTargets.Remove(TargetData.FakeTargetsUsed[i]);
			}

			PC->GetBreadCrumbsComponent()->AddFakeTargets(FakeTargets);
			PC->GetBreadCrumbsComponent()->SetBreadCrumbsVisible(true);
			PC->GetBreadCrumbsComponent()->CheckWaypoints();

			if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
			{
				APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
				PC2->GetBreadCrumbsComponent()->AddFakeTargets(FakeTargets);
				PC2->GetBreadCrumbsComponent()->SetBreadCrumbsVisible(true);
				PC2->GetBreadCrumbsComponent()->CheckWaypoints();
			}
		}
	}	
	
	CurrentQuest->SetQuestTargets(TargetData);
	OnQuestTargetUpdated.Broadcast(TargetData);
}

void UQuestSubsystem::OnQuestTargetLoaded(FGameplayTagContainer Target, AActor* ActorRef, FPDWQuestTargetData& TargetData, bool IsLoaded)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if(!PC)
		return;

	UPDWTagComponent* TagComponent = ActorRef->GetComponentByClass<UPDWTagComponent>();

	if (IsLoaded)
	{
		if (TargetData.bBreadCrumbsActive)
		{
			FPDWQuestTargetActor UpdatedTarget{};
			UpdatedTarget.TargetTag = Target;
			UpdatedTarget.TargetActor = ActorRef;
			UPDWBreadcrumbsTargetComponent* BreadcrumbsTarget = Cast<UPDWBreadcrumbsTargetComponent>(ActorRef->GetComponentByClass(UPDWBreadcrumbsTargetComponent::StaticClass()));
			if (BreadcrumbsTarget && ActorRef->GetRootComponent())
			{
				UpdatedTarget.BreadcrumbTargetOffset = BreadcrumbsTarget->GetComponentTransform()
					.GetRelativeTransform(ActorRef->GetRootComponent()->GetComponentTransform())
					.GetLocation();
			}

			PC->GetBreadCrumbsComponent()->AddBreadCrumbTarget(UpdatedTarget);
			//PC->GetBreadCrumbsComponent()->RemoveFakeTargets(ActorRef->GetActorLocation());
			if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
			{
				APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
				PC2->GetBreadCrumbsComponent()->AddBreadCrumbTarget(UpdatedTarget);
			}
		}

		if (TagComponent)
		{
			if (TargetData.bUsePaleoQuestIcon && !TagComponent->HasTag(UQuestSettings::GetQuestTargetTag()))
			{
				TagComponent->AddTag(UQuestSettings::GetPaleoQuestTargetTag());
			}
			else
			{
				if (TagComponent->HasTag(UQuestSettings::GetPaleoQuestTargetTag()))
				{
					TagComponent->RemoveTag(UQuestSettings::GetPaleoQuestTargetTag());
				}
				TagComponent->AddTag(UQuestSettings::GetQuestTargetTag());
			}
		}
	}
	else
	{
		if (TargetData.bBreadCrumbsActive)
		{
			PC->GetBreadCrumbsComponent()->RemoveBreadCrumbTarget(ActorRef);
			//PC->GetBreadCrumbsComponent()->AddFakeTargets(TArray<FVector> { ActorRef->GetActorLocation() });
			if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
			{
				APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
				PC2->GetBreadCrumbsComponent()->RemoveBreadCrumbTarget(ActorRef);
			}
		}

		if (TagComponent)
		{
			if (TargetData.bUsePaleoQuestIcon)
			{
				TagComponent->RemoveTag(UQuestSettings::GetPaleoQuestTargetTag());
			}
			else
			{
				TagComponent->RemoveTag(UQuestSettings::GetQuestTargetTag());
			}
		}
	}	
}

void UQuestSubsystem::RemoveQuestTarget(AActor* Target, FPDWQuestTargetData& TargetData)
{	
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC || !Target)
		return;

	UPDWTagComponent* TagComponent = Target->GetComponentByClass<UPDWTagComponent>();
	if (TagComponent)
	{
		if (TargetData.bUsePaleoQuestIcon)
		{
			TagComponent->RemoveTag(UQuestSettings::GetPaleoQuestTargetTag());
		}
		else
		{
			TagComponent->RemoveTag(UQuestSettings::GetQuestTargetTag());
		}

		if (!TargetData.IsMain)
			return;
		
		PC->GetBreadCrumbsComponent()->RemoveBreadCrumbTarget(Target);
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
		{
			APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
			PC2->GetBreadCrumbsComponent()->RemoveBreadCrumbTarget(Target);
		}

		UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
		if (!CurrentQuest)
			return;

		FPDWQuestStepData StepData = CurrentQuest->GetCurrentQuestStepData();
		StepData.TargetsData.CurrentQuantity++;
		StepData.TargetsData.FakeTargetsUsed.Add(TagComponent->QuestTargetID);

		CurrentQuest->SetCurrentQuestStepData(StepData);
		PC->GetBreadCrumbsComponent()->RemoveFakeTargets(TagComponent->QuestTargetID);
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
		{
			APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
			PC2->GetBreadCrumbsComponent()->RemoveFakeTargets(TagComponent->QuestTargetID);
		}
		OnQuestTargetUpdated.Broadcast(StepData.TargetsData);		
	}
}

void UQuestSubsystem::ClearBreadcrumbs()
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (PC)
	{
		PC->GetBreadCrumbsComponent()->ClearBreadCrumbTargets();
		PC->GetBreadCrumbsComponent()->SetBreadCrumbsVisible(false);
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
		{
			APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
			PC2->GetBreadCrumbsComponent()->ClearBreadCrumbTargets();
			PC2->GetBreadCrumbsComponent()->SetBreadCrumbsVisible(false);
		}
	}
}

void UQuestSubsystem::SetCurrentQuestDescription(FPDWStepDescriptionData DescriptionData)
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if (!CurrentQuest)
		return;
	
	CurrentQuest->SetQuestDescription(DescriptionData);
	OnQuestDescriptionUpdated.Broadcast(DescriptionData);
	UPDWDataFunctionLibrary::SetCurrentQuestDescription(this, DescriptionData.QuestDescription);
}

FPDWQuestStepData UQuestSubsystem::GetCurrentQuestStepData()
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if(CurrentQuest)
	{
		return CurrentQuest->GetCurrentStepData();
	}

	return FPDWQuestStepData();
}

FGameplayTag UQuestSubsystem::GetCurrentQuestID()
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if (!CurrentQuest)
		return FGameplayTag::EmptyTag;
	
	return CurrentQuest->GetQuestID();
}

FText UQuestSubsystem::GetCurrentQuestName()
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if (!CurrentQuest)
		return FText();
	
	return CurrentQuest->GetQuestName();
}

TArray<FText> UQuestSubsystem::GetCurrentQuestDescriptionHistory()
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if (!CurrentQuest)
		return TArray<FText>();
	
	return TArray<FText>(); //CurrentQuest->GetQuestDescriptionHistory();
}

void UQuestSubsystem::LoadStep()
{
	UBaseFlowQuest* CurrentQuest = GetCurrentQuest();
	if(CurrentQuest)
	{
		StartStep(GetCurrentQuest()->GetCurrentQuestStepData());
	}
}

void UQuestSubsystem::ClearQuests()
{
	ActiveQuests.Empty();
}

void UQuestSubsystem::Deinitialize()
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnObservedActorLoaded.RemoveDynamic(this, &UQuestSubsystem::OnObservedActorLoaded);
		EventSubsystem->OnObservedActorUnloaded.RemoveDynamic(this, &UQuestSubsystem::OnObservedActorUnloaded);
		EventSubsystem->OnMultiplayerStateChange.RemoveDynamic(this, &UQuestSubsystem::OnMultiplayerStateChange);
	}

	Super::Deinitialize();
}

UPDWDialogueFlowAsset* UQuestSubsystem::GetCurrentQuestDialogueFlow()
{
	return CurrentDialogFlowAsset;
}

void UQuestSubsystem::SetCurrentQuestDialogueFlow(UPDWDialogueFlowAsset* inFlow)
{
	CurrentDialogFlowAsset = inFlow;
}

FString UQuestSubsystem::GetQuestIndexByTag(FGameplayTag QuestTag)
{
	TArray<FString> Temp;
	QuestTag.ToString().ParseIntoArray(Temp, TEXT("."), true);
	return Temp.Num() > 1 ? Temp[1] : "";
}

void UQuestSubsystem::InitGameplay()
{
	UQuestSettings* QuestSettings = UQuestSettings::Get();
	QuestConfig = Cast<UPDWQuestConfig>(QuestSettings->QuestConfig.TryLoad());
	if(!QuestConfig)
		return;

	ClearQuests();
		
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnObservedActorLoaded.AddUniqueDynamic(this, &UQuestSubsystem::OnObservedActorLoaded);
		EventSubsystem->OnObservedActorUnloaded.AddUniqueDynamic(this, &UQuestSubsystem::OnObservedActorUnloaded);
		EventSubsystem->OnMultiplayerStateChange.AddUniqueDynamic(this, &UQuestSubsystem::OnMultiplayerStateChange);
	}
}

FPDWWaypointData UQuestSubsystem::GetWaypointsByArea(FGameplayTag AreaTag)
{
	return WaypointsData.Contains(AreaTag) ? WaypointsData[AreaTag] : FPDWWaypointData();
}

void UQuestSubsystem::AddWaypoint(APDW_NavigationWaypoint* SpawnedWaypoint)
{
	if(!SpawnedWaypoint)
	return;

	if (WaypointsData.Contains(SpawnedWaypoint->AreaTag))
	{
		WaypointsData[SpawnedWaypoint->AreaTag].Waypoints.AddUnique(SpawnedWaypoint);
	}
	else
	{
		FPDWWaypointData NewList;
		NewList.Waypoints.Add(SpawnedWaypoint);
		WaypointsData.Add(SpawnedWaypoint->AreaTag, NewList);
	}

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	if (GM && GM->IsGameReady())
	{
		APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
		if (PC)
		{
			PC->GetBreadCrumbsComponent()->CheckWaypoints();
		}
		APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
		if (PC2)
		{
			PC2->GetBreadCrumbsComponent()->CheckWaypoints();
		}
	}
}

void UQuestSubsystem::OnObservedActorLoaded(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator)
{
	if(!Component) return;
	OnQuestTargetLoaded(Component->IdentityTags, Actor,TargetData, true);
}

void UQuestSubsystem::OnObservedActorUnloaded(AActor* Actor, UFlowComponent* Component, FPDWQuestTargetData& TargetData, UFlowNode* Instigator)
{
 	if(!Component) return;
	OnQuestTargetLoaded(Component->IdentityTags, Actor, TargetData, false);
}

void UQuestSubsystem::OnMultiplayerStateChange()
{
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
	{
		APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
		APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
		if (PC && PC2 && PC2->GetBreadCrumbsComponent() && PC->GetBreadCrumbsComponent())
		{
			PC2->GetBreadCrumbsComponent()->SetBreadCrumbsVisible(PC->GetBreadCrumbsComponent()->GetBreadCrumbsVisible());
			PC2->GetBreadCrumbsComponent()->AddFakeTargets(PC->GetBreadCrumbsComponent()->GetFakeTargets());
			TArray<FPDWQuestTargetActor> Targets = PC->GetBreadCrumbsComponent()->GetBreadCrumbTargets();
			for (FPDWQuestTargetActor Target : Targets)
			{
				PC2->GetBreadCrumbsComponent()->AddBreadCrumbTarget(Target);
			}
			PC2->GetBreadCrumbsComponent()->CheckWaypoints();
		}
	}
}
