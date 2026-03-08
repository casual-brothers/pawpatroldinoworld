// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/PDWCollectQuestTargetLocations.h"
#include "EngineUtils.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "FlowComponent.h"
#include "FlowQuest/Public/QuestSettings.h"
#include "Modes/PDWGameplayGameMode.h"
#include "FlowAsset.h"
#include "Gameplay/FLOW/PDWFlowNode_UpdateQuestStep.h"
#include "Gameplay/FLOW/PDWFlowNode_Quest.h"
#include "Gameplay/FLOW/PDWFlowNode_OnNotifyFromMinigame.h"
#include "Gameplay/FLOW/PDWFlowNode_OnInteractionFinish.h"
#include "Gameplay/FLOW/PDWFlowNode_OnTriggerEnter.h"
#include "Gameplay/FLOW/PDWFlowNode_PickUp.h"
#include "Gameplay/Components/PDWTagComponent.h"
#include "Gameplay/Components/PDWBreadcrumbsTargetComponent.h"
#include "Gameplay/Interaction/PDWStartMiniGameBehaviour.h"

void UPDWCollectQuestTargetLocations::CollectQuestTargetActors()
{
    if (!GEditor) return;

    UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
    if (!EditorWorld) return;

	TArray<FPDWQuestTargetLocs> TargetLocations = {};

	TArray<UFlowNode*> AllQuests = RootFlowAsset->GetNodesInExecutionOrder(RootFlowAsset->GetDefaultEntryNode(), UPDWFlowNode_Quest::StaticClass());
	
	for (int32 i = 0; i < AllQuests.Num(); i++)
	{
		UFlowNode_SubGraph* QuestSubGraph = Cast<UFlowNode_SubGraph>(AllQuests[i]);
		UFlowAsset* QuestFlowAsset = QuestSubGraph->GetSubGraphAsset().LoadSynchronous();
		TArray<UFlowNode*> AllNodes = QuestFlowAsset->GetNodesInExecutionOrder(QuestFlowAsset->GetDefaultEntryNode(), UPDWFlowNode_OnNotifyFromMinigame::StaticClass());
		AllNodes.Append(QuestFlowAsset->GetNodesInExecutionOrder(QuestFlowAsset->GetDefaultEntryNode(), UPDWFlowNode_OnInteractionFinish::StaticClass()));
		AllNodes.Append(QuestFlowAsset->GetNodesInExecutionOrder(QuestFlowAsset->GetDefaultEntryNode(), UPDWFlowNode_OnTriggerEnter::StaticClass()));
		AllNodes.Append(QuestFlowAsset->GetNodesInExecutionOrder(QuestFlowAsset->GetDefaultEntryNode(), UPDWFlowNode_PickUp::StaticClass()));

		for (int32 j = 0; j < AllNodes.Num(); j++)
		{			
			if (UPDWFlowNode_OnNotifyFromMinigame* NotifyFromMinigameNode = Cast<UPDWFlowNode_OnNotifyFromMinigame>(AllNodes[j]))
			{
				FPDWQuestTargetLocs NewLoc;
				NewLoc.TagContainer = NotifyFromMinigameNode->GetIdentityTags();
				NewLoc.MatchType = NotifyFromMinigameNode->GetIdentityMatchType();
				NewLoc.MinigameLayersInfo.Append(NotifyFromMinigameNode->OnEnterDataLayersInfo);
				NewLoc.MinigameLayersInfo.Append(NotifyFromMinigameNode->MinigameInitCompleteLayersInfo);
				TargetLocations.Add(NewLoc);
			}
			else if (UPDWFlowNode_OnInteractionFinish* InteractionFinishNode = Cast<UPDWFlowNode_OnInteractionFinish>(AllNodes[j]))
			{
				FPDWQuestTargetLocs NewLoc;
				NewLoc.TagContainer = InteractionFinishNode->GetIdentityTags();
				NewLoc.MatchType = InteractionFinishNode->GetIdentityMatchType();
				TargetLocations.Add(NewLoc);		
			}
			else if (UPDWFlowNode_OnTriggerEnter* TriggerEnterNode = Cast<UPDWFlowNode_OnTriggerEnter>(AllNodes[j]))
			{
				FPDWQuestTargetLocs NewLoc;
				NewLoc.TagContainer = TriggerEnterNode->GetIdentityTags();
				NewLoc.MatchType = TriggerEnterNode->GetIdentityMatchType();
				TargetLocations.Add(NewLoc);		
			}
			else if (UPDWFlowNode_PickUp* PickUpNode = Cast<UPDWFlowNode_PickUp>(AllNodes[j]))
			{
				FPDWQuestTargetLocs NewLoc;
				NewLoc.TagContainer = PickUpNode->GetIdentityTags();
				NewLoc.MatchType = PickUpNode->GetIdentityMatchType();
				TargetLocations.Add(NewLoc);		
			}
		}
	}

	for (TActorIterator<AActor> It(EditorWorld); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		UFlowComponent* FlowComp = Cast<UFlowComponent>(Actor->GetComponentByClass(UFlowComponent::StaticClass()));
		if (FlowComp)
		{
			for (FPDWQuestTargetLocs& TargetLoc : TargetLocations)
			{
				if (FlowTypes::HasMatchingTags(FlowComp->IdentityTags, TargetLoc.TagContainer, TargetLoc.MatchType)) 
				{
					UPDWMinigameConfigComponent* MinigameConfigComp = Cast<UPDWMinigameConfigComponent>(Actor->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
					if (MinigameConfigComp) 
					{
						UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(Actor->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
						if (!InteractionComp) return;

						TArray<TObjectPtr<UPDWInteractionBehaviour>> BehaviourValues = {};
						TSubclassOf<UPDWInteractionReceiverConfigData> DataConfig = InteractionComp->GetDataConfiguration();
						UPDWInteractionReceiverConfigData* InteractionReceiverData = NewObject<UPDWInteractionReceiverConfigData>(this, DataConfig);

						TMap<FGameplayTag, TObjectPtr<UPDWInteractionBehaviour>> Behaviours = InteractionReceiverData->Behaviours;
						Behaviours.GenerateValueArray(BehaviourValues);
						if (BehaviourValues.Num() == 0) return;

						TMap<FGameplayTag,TSubclassOf<UPDWMinigameConfigData>> MinigameConfigurationData = MinigameConfigComp->GetStateMiniGameConfigurationData();
						if (!MinigameConfigurationData.Contains(InteractionReceiverData->StartingState)) return;

						UPDWMinigameConfigData* MinigameConfig = NewObject<UPDWMinigameConfigData>(this, MinigameConfigurationData[InteractionReceiverData->StartingState]);
						UPDWStartMiniGameBehaviour* StartMinigameBehaviour = Cast<UPDWStartMiniGameBehaviour>(BehaviourValues[0].Get());
						if (!StartMinigameBehaviour || !MinigameConfig) return;

						TargetLoc.Name = MinigameConfig->MiniGameDisplayName;
						TargetLoc.AllowedPup = StartMinigameBehaviour->GetAllowedPup();
					}

					UPDWTagComponent* TagComponent = Actor->FindComponentByClass<UPDWTagComponent>();
					if (TagComponent)
					{
						FVector TargetLocation =  Actor->GetActorLocation();

						UPDWBreadcrumbsTargetComponent* BreadcrumbsTarget = Cast<UPDWBreadcrumbsTargetComponent>(Actor->GetComponentByClass(UPDWBreadcrumbsTargetComponent::StaticClass()));
						if (BreadcrumbsTarget && Actor->GetRootComponent())
						{
							FVector BreadcrumbOffset = BreadcrumbsTarget->GetComponentTransform()
								.GetRelativeTransform(Actor->GetRootComponent()->GetComponentTransform())
								.GetLocation();

							TargetLocation = Actor->GetActorLocation() + Actor->GetActorRotation().RotateVector(BreadcrumbOffset);
						}
						TargetLoc.Locations.Emplace(TagComponent->QuestTargetID, TargetLocation);
					}
				}
			}
		}		
	}
	
	for (FPDWQuestTargetLocs Location : TargetLocations)
	{
		if (Location.Locations.Num() == 0)
		{
			FText Template = NSLOCTEXT("QuestTargetLocs", "Error", "{0} non trovato! controlla se ha il tagcomponent e se il datalayer e' attivo in editor");
			FText TargetName = FText::FromString(Location.TagContainer.ToString());
			FText Message = FText::Format(Template, TargetName);
			FMessageDialog::Open(EAppMsgType::Ok, Message);
			break;
		}
		else if (Location.Locations.Contains(-1))
		{
			FText Template = NSLOCTEXT("QuestTargetLocs", "Error", "{0} ha un id -1! usa l'AssignID tool!");
			FText TargetName = FText::FromString(Location.TagContainer.ToString());
			FText Message = FText::Format(Template, TargetName);
			FMessageDialog::Open(EAppMsgType::Ok, Message);
			break;
		}

	}

	//Save in DA
	if (QuestConfig)
	{
		QuestConfig->TargetLocations = TargetLocations;
		QuestConfig->MarkPackageDirty();
	}
}