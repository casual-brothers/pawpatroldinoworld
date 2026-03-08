// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWCheatManager.h"
#include "Data/PDWLocalPlayer.h"
#include "FlowSave.h"
#include "FlowSubsystem.h"
#include "FlowAsset.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FlowSubsystem.h"
#include "Data/PDWPersistentUser.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/FLOW/PDWFlowNode_Quest.h"
#include "Managers/QuestSubsystem.h"
#include "Gameplay/FLOW/PDWFlowNode_UpdateQuestStep.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "NavigationSystem.h"
#include "Gameplay/Actors/PDWDinoPen.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/FLOW/PDWFlowNode_OnNotifyFromMinigame.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "QuestSettings.h"
#include "WPHelperSubsystem.h"
#include "Nodes/Route/FlowNode_ExecutionSequence.h"
#include "Gameplay/Components/PDWBreadCrumbsComponent.h"
#include "Gameplay/FLOW/Dialogue/PDWFlowNode_NotifyQuestState.h"
#include "Nodes/Graph/FlowNode_Finish.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueSubGraphNode.h"
#include "UI/NebulaFlowBasePage.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Gameplay/Actors/PDWMovingNPCActorsSpawner.h"
#include "PDWGameInstance.h"
#include "Data/PDWSaveStructures.h"
#include "UI/Widgets/CLDebugWidget.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Engine/GameViewportClient.h"
#include "../../../../../../../Source/Runtime/Engine/Public/UnrealClient.h"
#include "../../../../../../../Source/Runtime/Slate/Public/Widgets/SViewport.h"

void UPDWCheatManager::PDW_TestSave()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!GameInstance)
		return;
	UPDWLocalPlayer* LocalPlayer = Cast<UPDWLocalPlayer>(UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this));
	if (!LocalPlayer)
		return;

	UPDWPersistentUser* PersistentUser = UPDWDataFunctionLibrary::GetPersistentUser(this);

	if (PersistentUser)
	{
		if (UFlowSubsystem* FlowSubsystem = GameInstance->GetSubsystem<UFlowSubsystem>())
		{
			UFlowSaveGame* NewSaveGame = Cast<UFlowSaveGame>(UGameplayStatics::CreateSaveGameObject(UFlowSaveGame::StaticClass()));
			FlowSubsystem->OnGameSaved(NewSaveGame);
			UPDWDataFunctionLibrary::SetFlowSaveGame(GetWorld(), NewSaveGame);

			PersistentUser->OnBeforeAutoSave();
			bool bResult;
			LocalPlayer->SavePersistentUser(bResult);
			PersistentUser->WaitForSaveComplete();
		}
	}
}

void UPDWCheatManager::PDW_TestLoad(int32 SlotIndex)
{
	UPDWPersistentUser* SaveGame = UPDWDataFunctionLibrary::GetPersistentUser(this);
	UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (SaveGame && localPlayerOwner && GInstance)
	{
		if (SlotIndex >= SaveGame->GetTotalSlotNum())
		{
			return;
		}
		GInstance->OnPersistentUserLoadedDelegate.AddDynamic(this, &UPDWCheatManager::PDW_PostLoadTest);
		if (localPlayerOwner)
		{
			CurrentLoadedSlotID = SlotIndex;
			localPlayerOwner->LoadPersistentUser();
		}
	}
}

void UPDWCheatManager::PDW_PostLoadTest(int UserID)
{
	UPDWPersistentUser* SaveGame = UPDWDataFunctionLibrary::GetPersistentUser(this);
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (CurrentLoadedSlotID >= 0 && SaveGame && GInstance)
	{
		GInstance->OnPersistentUserLoadedDelegate.RemoveDynamic(this, &UPDWCheatManager::PDW_PostLoadTest);
		SaveGame->OnPostLoadFromSlot(CurrentLoadedSlotID);
		CurrentLoadedSlotID = -1;
		UNebulaFlowCoreFunctionLibrary::LoadLevelByName(this, FName(UGameplayStatics::GetCurrentLevelName(this)));
	}
}

void UPDWCheatManager::PDW_ShowCurrentSlotID()
{
	//NON COMPILA SWITCH PER QUESTA FUNZIONE
	/*UPDWPersistentUser* SaveGame = UPDWDataFunctionLibrary::GetSaveGame(this);
	if (SaveGame)
	{
		FGameProgressionSlot ProgressionSlot = SaveGame->GetCurrentSavedDataStructure(this);
		ProgressionSlot.SlotMetaData.SlotID;
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Current loaded slot: %d"), ProgressionSlot.SlotMetaData.SlotID));
	}*/
}

TArray<UFlowNode*> UPDWCheatManager::GetNodesToSkip(UPDWFlowNode_Quest* QuestNode, UClass* TargetNodeClass, bool& bFound, FGameplayTagContainer TagToFind)
{
	UFlowAsset* SubGraphAsset = QuestNode->GetSubGraphAsset().Get();
	UBaseFlowQuest* QuestSubGraphAsset = Cast<UBaseFlowQuest>(SubGraphAsset);
	TSoftObjectPtr<UFlowAsset> FlowAsset = QuestNode->GetFlowAsset();
	TWeakObjectPtr<UFlowAsset> QuestSubGraph = FlowAsset->GetFlowInstance(QuestNode);
	TArray<UFlowNode*> ActiveNodes = QuestSubGraph->GetActiveNodes().Num() > 0 ? QuestSubGraph->GetActiveNodes() : TArray<UFlowNode*>{ QuestSubGraph->GetDefaultEntryNode() };
	TArray<UFlowNode*> AllNodes = {};
	TArray<UFlowNode*> AllTargetNode = QuestSubGraph->GetNodesInExecutionOrder(QuestSubGraph->GetDefaultEntryNode(), TargetNodeClass);
	UFlowNode* TargetNode = AllTargetNode.Num() > 0 ? AllTargetNode[0] : nullptr;
	if (!TargetNode) return TArray<UFlowNode*>();

	TArray<UFlowNode*> MissingBranches = {};
	bool FinishNodeReached = false;

	for (UFlowNode* ActiveNode : ActiveNodes)
	{
		if (ActiveNode->IsA(UPDWDialogueSubGraphNode::StaticClass())) continue;

		TArray<UFlowNode*> NextNodes = QuestSubGraph->GetNodesInExecutionOrder(ActiveNode, UFlowNode::StaticClass());
		for (UFlowNode* NextNode : NextNodes)
		{
			if (NextNode->IsA(UPDWDialogueSubGraphNode::StaticClass())) continue;
			UFlowNode_ComponentObserver* OberservNode = Cast<UFlowNode_ComponentObserver>(NextNode);
			if (!bFound && TargetNode->GetClass() == NextNode->GetClass() && (TagToFind == FGameplayTagContainer::EmptyContainer || (OberservNode && OberservNode->GetIdentityTags().HasAllExact(TagToFind))))
			{
				bFound = true;
			}

			if (bFound)
			{
				if (FinishNodeReached)
				{
					if (AllNodes.Contains(NextNode))
					{
						break;
					}
					else if (NextNode->GetActivationState() == EFlowNodeState::Active)
					{
						//if i am here there are branches behind, remove connections and passthrough
						TMap<FName, FConnectedPin> InConnections = {};
						NextNode->SetConnections(InConnections);
						NextNode->SetCheatMode(true);
						NextNode->ForceSignalMode(EFlowSignalMode::PassThrough);
						NextNode->OnPassThrough_Implementation();
					}
				}
				if (Cast<UFlowNode_Finish>(NextNode))
				{
					FinishNodeReached = true;
				}
			}
			else if (!AllNodes.Contains(NextNode))
			{
				NextNode->SetCheatMode(true);
				NextNode->ForceSignalMode(EFlowSignalMode::PassThrough);
				AllNodes.Add(NextNode);
			}
		}

		//Dead end branch
		if (!bFound)
		{
			AllNodes[0]->OnPassThrough_Implementation();
			AllNodes.Empty();
		}
	}

	return AllNodes;
}

void UPDWCheatManager::PDW_SaveAndRestore()
{
	PDW_TestSave();
	UNebulaFlowBasePage* Page = UNebulaFlowUIFunctionLibrary::GetCurrentPage(GetWorld());
	if (Page)
	{
		UNebulaFlowUIFunctionLibrary::RemovePage(GetWorld(), Page);
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
}


void UPDWCheatManager::PDW_EndCurrentQuest()
{
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		TMap<FGameplayTag, UBaseFlowQuest*> ActiveQuests = QuestSubsystem->GetActiveQuests();
		for (TPair<FGameplayTag, UBaseFlowQuest*> ActiveQuest : ActiveQuests)
		{
			if (!ActiveQuest.Value)
				continue;

			TObjectPtr<UFlowComponent> RootFlow = UPDWGameplayFunctionLibrary::GetQuestManagerComp(this);
			UFlowAsset* RootFlowAsset = RootFlow->GetRootFlowInstance();
			TArray<UFlowNode*> RootFlowActiveNodes = RootFlowAsset->GetActiveNodes();
			if (RootFlowActiveNodes.IsValidIndex(RootFlowActiveNodes.Num() - 1) && RootFlowActiveNodes[RootFlowActiveNodes.Num() - 1])
			{
				UPDWFlowNode_Quest* QuestNode = Cast<UPDWFlowNode_Quest>(RootFlowActiveNodes[0]);
				TArray<UFlowNode*> NodesToSkip = {};
				bool bFound = false;
				NodesToSkip.Append(GetNodesToSkip(QuestNode, UFlowNode_Finish::StaticClass(), bFound));
				if (bFound)
				{
					ClearBreadcrumbs();
					NodesToSkip[0]->OnPassThrough_Implementation();
					break;
				}
				else
				{
					PDW_EndCurrentQuest();
					return;
				}
			}
		}
	}
}

void UPDWCheatManager::PDW_EndCurrentStep()
{
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		TMap<FGameplayTag, UBaseFlowQuest*> ActiveQuests = QuestSubsystem->GetActiveQuests();
		for (TPair<FGameplayTag, UBaseFlowQuest*> ActiveQuest : ActiveQuests)
		{
			if (!ActiveQuest.Value)
				continue;

			TObjectPtr<UFlowComponent> RootFlow =  UPDWGameplayFunctionLibrary::GetQuestManagerComp(this);
			UFlowAsset* RootFlowAsset = RootFlow->GetRootFlowInstance();
			TArray<UFlowNode*> RootFlowActiveNodes = RootFlowAsset->GetActiveNodes();
			if (RootFlowActiveNodes.IsValidIndex(RootFlowActiveNodes.Num() - 1) && RootFlowActiveNodes[RootFlowActiveNodes.Num() - 1])
			{
				UPDWFlowNode_Quest* QuestNode = Cast<UPDWFlowNode_Quest>(RootFlowActiveNodes[0]);
				TArray<UFlowNode*> NodesToSkip = {};
				bool bFound = false;
				NodesToSkip.Append(GetNodesToSkip(QuestNode, UPDWFlowNode_UpdateQuestStep::StaticClass(), bFound));
				if (bFound)
				{
					ClearBreadcrumbs();
					NodesToSkip[0]->OnPassThrough_Implementation();
					break;
				}
				else
				{
					ClearBreadcrumbs();
					return;
				}
			}
		}
	}
}

void UPDWCheatManager::PDW_StartQuest(FString _QuestToStart)
{
	FGameplayTag QuestToStart = BuildFullTag(_QuestToStart, QuestTagPrefix);

	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		TObjectPtr<UFlowComponent> RootFlow =  UPDWGameplayFunctionLibrary::GetQuestManagerComp(this);
		UFlowAsset* RootFlowAsset = RootFlow->GetRootFlowInstance();
		TArray<UFlowNode*> RootFlowActiveNodes = RootFlowAsset->GetActiveNodes();
		UPDWFlowNode_Quest* TargetQuest = nullptr;

		if (RootFlowActiveNodes.IsValidIndex(RootFlowActiveNodes.Num() - 1) && RootFlowActiveNodes[RootFlowActiveNodes.Num() - 1])
		{
			TArray<UFlowNode*> AllNoQuestNodes = RootFlowAsset->GetNodesInExecutionOrder(RootFlowActiveNodes[RootFlowActiveNodes.Num() - 1], UFlowNode::StaticClass());
			TArray<UFlowNode*> NodesToSkip = {};

			for (int32 i = 0; i < AllNoQuestNodes.Num(); i++)
			{
				if (AllNoQuestNodes[i]->IsA(UPDWFlowNode_Quest::StaticClass()))
				{
					UPDWFlowNode_Quest* QuestNode = Cast<UPDWFlowNode_Quest>(AllNoQuestNodes[i]);

					if (QuestNode->GetActivationState() == EFlowNodeState::NeverActivated)
					{
						QuestNode->Cheat_CreateQuest();
					}

					UFlowAsset* SubGraphAsset = QuestNode->GetSubGraphAsset().Get();
					UBaseFlowQuest* QuestSubGraphAsset = Cast<UBaseFlowQuest>(SubGraphAsset);
					bool bFound = false;

					if (QuestSubGraphAsset && QuestSubGraphAsset->GetQuestID().MatchesTag(QuestToStart))
					{
						TargetQuest = QuestNode;
						NodesToSkip.Append(GetNodesToSkip(QuestNode, UPDWFlowNode_NotifyQuestState::StaticClass(), bFound));
						break;
					}
					else
					{
						//NodesToSkip.Add(AllNoQuestNodes[i]);
						NodesToSkip.Append(GetNodesToSkip(QuestNode, UFlowNode_Finish::StaticClass(), bFound));
					}
				}
				else
				{
					NodesToSkip.Add(AllNoQuestNodes[i]);
				}
			}

			ClearBreadcrumbs();

			if (NodesToSkip.Num() > 0)
			{
				NodesToSkip[0]->OnPassThrough_Implementation();
			}
			//ResumeQuestFlow(TargetQuest, NodesToSkip);
		}
	}
}

void UPDWCheatManager::PDW_GoToQuestMinigame(FString _MinigameTag, FString _QuestTag, float TeleportHeight)
{
	FGameplayTag MinigameTag = BuildFullTag(_MinigameTag, MinigameTagPrefix);
	FGameplayTag QuestTag = BuildFullTag(_QuestTag, QuestTagPrefix);

	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		TObjectPtr<UFlowComponent> RootFlow =  UPDWGameplayFunctionLibrary::GetQuestManagerComp(this);
		UFlowAsset* RootFlowAsset = RootFlow->GetRootFlowInstance();
		TArray<UFlowNode*> RootFlowActiveNodes = RootFlowAsset->GetActiveNodes();
		UPDWFlowNode_Quest* TargetQuest = nullptr;
		FGameplayTagContainer TagContainer = {};

		if (RootFlowActiveNodes.IsValidIndex(RootFlowActiveNodes.Num() - 1) && RootFlowActiveNodes[RootFlowActiveNodes.Num() - 1])
		{
			TArray<UFlowNode*> AllNoQuestNodes = RootFlowAsset->GetNodesInExecutionOrder(RootFlowActiveNodes[RootFlowActiveNodes.Num() - 1], UFlowNode::StaticClass());
			TArray<UFlowNode*> NodesToSkip = {};

			for (int32 i = 0; i < AllNoQuestNodes.Num(); i++)
			{
				if (AllNoQuestNodes[i]->IsA(UPDWFlowNode_Quest::StaticClass()))
				{
					UPDWFlowNode_Quest* QuestNode = Cast<UPDWFlowNode_Quest>(AllNoQuestNodes[i]);

					if (QuestNode->GetActivationState() == EFlowNodeState::NeverActivated)
					{
						QuestNode->Cheat_CreateQuest();
					}

					TSoftObjectPtr<UFlowAsset> FlowAsset = QuestNode->GetFlowAsset();
					TWeakObjectPtr<UFlowAsset> QuestSubGraph = FlowAsset->GetFlowInstance(QuestNode);

					if (QuestSubGraph != nullptr)
					{
						bool bFound = false;
						TagContainer = {};
						TagContainer.AddTagFast(MinigameTag);
						TagContainer.AddTagFast(QuestTag);
						NodesToSkip.Append(GetNodesToSkip(QuestNode, UPDWFlowNode_OnNotifyFromMinigame::StaticClass(), bFound, TagContainer));
						if (bFound)
						{
							TargetQuest = QuestNode;
							break;
						}
					}
				}
				else
				{
					NodesToSkip.Add(AllNoQuestNodes[i]);
				}
			}

			if (NodesToSkip.Num() > 0)
			{
				NodesToSkip[0]->OnPassThrough_Implementation();
			}
		}

		ReachMinigameLocation(TagContainer, TeleportHeight);
	}
}

void UPDWCheatManager::PDW_UnlockDinoPen(FString _PenToUnlock)
{
	FGameplayTag PenToUnlock = BuildFullTag(_PenToUnlock, DinoPenTagPrefix);

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->UnlockDinoPen(PenToUnlock);
	}
}

void UPDWCheatManager::PDW_UnlockAllDinoPens()
{
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->UnlockDinoPen(UGameplayTagsManager::Get().RequestGameplayTag("ID.DinoPen.Brachiosaurus", false));
		PaleoCenterSubsystem->UnlockDinoPen(UGameplayTagsManager::Get().RequestGameplayTag("ID.DinoPen.Triceratops", false));
		PaleoCenterSubsystem->UnlockDinoPen(UGameplayTagsManager::Get().RequestGameplayTag("ID.DinoPen.Stegosaurus", false));
		PaleoCenterSubsystem->UnlockDinoPen(UGameplayTagsManager::Get().RequestGameplayTag("ID.DinoPen.Mahakala", false));
	}
}

void UPDWCheatManager::PDW_ChangeDinoPenActivity(FString _PenTag, bool IsActive)
{
	FGameplayTag PenTag = BuildFullTag(_PenTag, DinoPenTagPrefix);

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->ChangeDinoPenActivity(PenTag, IsActive);
	}
}

void UPDWCheatManager::PDW_ForceNavMeshRebuild()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		NavSys->Build();
	}
}

void UPDWCheatManager::PDW_ForceDinoNeed(FString _PenTag, FString _NeedTag)
{
	FGameplayTag PenTag = BuildFullTag(_PenTag, DinoPenTagPrefix);
	FGameplayTag NeedTag = BuildFullTag(_NeedTag, DinoNeedTagPrefix);

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		TArray<UPDWDinoPen*> DinoPens = PaleoCenterSubsystem->GetDinoPens();
		for (UPDWDinoPen* DinoPen : DinoPens)
		{
			if (DinoPen->DinoPenTag == PenTag)
			{
				DinoPen->UpdateDinoNeeds(NeedTag);
			}
		}
	}
}

void UPDWCheatManager::PDW_UnlockEggs(FString _PenTag)
{
	FGameplayTag PenTag = BuildFullTag(_PenTag, DinoPenTagPrefix);
	FString ItemID = "ID.Collectible.Egg.";
	ItemID.Append(_PenTag);
	PDW_AddItemToInventory(FGameplayTag::RequestGameplayTag(FName(ItemID.Append(".01"))), 1);
	ItemID.RemoveFromEnd(".01");
	PDW_AddItemToInventory(FGameplayTag::RequestGameplayTag(FName(ItemID.Append(".02"))), 1);
	ItemID.RemoveFromEnd(".02");
	PDW_AddItemToInventory(FGameplayTag::RequestGameplayTag(FName(ItemID.Append(".03"))), 1);	
	ItemID.RemoveFromEnd(".03");
	PDW_AddItemToInventory(FGameplayTag::RequestGameplayTag(FName(ItemID.Append(".04"))), 1);	
	ItemID.RemoveFromEnd(".04");
	PDW_AddItemToInventory(FGameplayTag::RequestGameplayTag(FName(ItemID.Append(".05"))), 1);	
	ItemID.RemoveFromEnd(".05");
}

void UPDWCheatManager::PDW_AddItemToInventory(FGameplayTag ItemTag, int32 Quantity)
{
	UPDWDataFunctionLibrary::AddItemToInventory(this, ItemTag, Quantity);

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnItemCollectedEvent(ItemTag);
	}
}

void UPDWCheatManager::PDW_AddExp(int32 ExpToAdd)
{
	APDWPlayerController* PlayerController = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (PlayerController && PlayerController->PlayerState)
	{
		Cast<APDWPlayerState>(PlayerController->PlayerState)->ChangePupTreats(ExpToAdd);
	}
}

void UPDWCheatManager::PDW_GetAllUnlockables()
{
	FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(FName("ID.Collectible.Cosmetic"));
    FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(ParentTag);
	TArray<FGameplayTag> LeafTags;

	for (const FGameplayTag& Tag : TagContainer)
	{
		FGameplayTagContainer SubChildren = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
		if (SubChildren.Num() == 0)
		{
			LeafTags.Add(Tag);
		}
	}

    for (const FGameplayTag& Tag : LeafTags)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Unlocked: %s"), *Tag.ToString()));
		UPDWDataFunctionLibrary::AddItemToInventory(this, Tag, 1);

		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnItemCollectedEvent(Tag);
		}
    }
}

void UPDWCheatManager::PDW_SetBreadcrumbsDebug(bool IsActive)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC)
		return;
	PC->GetBreadCrumbsComponent()->ShowDebug = IsActive;

	APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
	if (!PC2)
		return;
	PC2->GetBreadCrumbsComponent()->ShowDebug = IsActive;
}

void UPDWCheatManager::PDW_SetWaypointsDebug(bool IsActive)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC)
		return;
	PC->GetBreadCrumbsComponent()->ShowWaypointDebug = IsActive;

	APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
	if (!PC2)
		return;
	PC2->GetBreadCrumbsComponent()->ShowWaypointDebug = IsActive;
}

void UPDWCheatManager::PDW_UnlockAllAreas()
{
	UPDWPersistentUser* SaveGame = UPDWDataFunctionLibrary::GetPersistentUser(this);
	if (SaveGame)
	{
		bool bSuccess = false;
		FGameProgressionSlot& SavedData = SaveGame->GetCurrentUsedProgressionSlot(bSuccess);
		SavedData.PlayerData.VisitedAreaIds.AddUnique(FGameplayTag::RequestGameplayTag("Map.DinoPlains"));
		SavedData.PlayerData.VisitedAreaIds.AddUnique(FGameplayTag::RequestGameplayTag("Map.DinoShores"));
		SavedData.PlayerData.VisitedAreaIds.AddUnique(FGameplayTag::RequestGameplayTag("Map.Jungle"));
		SavedData.PlayerData.VisitedAreaIds.AddUnique(FGameplayTag::RequestGameplayTag("Map.Volcano"));
	}
}

void UPDWCheatManager::TestGameModal()
{
	GameTestDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, "AreYouSure", [&](FString Response) {
		if (Response == "Yes")
		{
			UNebulaFlowUIFunctionLibrary::HideDialog(this, GameTestDialog);
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Game Modal " + Response));
		});
}

void UPDWCheatManager::TestSystemModal()
{
	SystemTestDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, "SAVE_FAILED_LOOP", [&](FString Response) {
		UNebulaFlowUIFunctionLibrary::HideDialog(this, SystemTestDialog);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("System Modal " + Response));
		});
}

void UPDWCheatManager::PDW_ShowGameUI()
{
	const UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return;
	}

	UGameViewportClient* ViewportClient = World->GetGameViewport();
	if (IsValid(ViewportClient) == false)
	{
		return;
	}
	
	TSharedPtr<SViewport> GameViewportWidget = ViewportClient->GetGameViewportWidget();
	if (GameViewportWidget.IsValid())
	{
		GameViewportWidget->SetRenderOpacity(1.0f);
	}
}

void UPDWCheatManager::PDW_HideGameUI()
{
	const UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return;
	}

	UGameViewportClient* ViewportClient = World->GetGameViewport();
	if (IsValid(ViewportClient) == false)
	{
		return;
	}
	
	TSharedPtr<SViewport> GameViewportWidget = ViewportClient->GetGameViewportWidget();
	if (GameViewportWidget.IsValid())
	{
		GameViewportWidget->SetRenderOpacity(0.0f);
	}
}

void UPDWCheatManager::PDW_HideBuildInfo()
{
	const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	ensure(GameMode);
	GameMode->ClPage->HideCL();
}

void UPDWCheatManager::PDW_ShowBuildInfo()
{
	const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	ensure(GameMode);
	GameMode->ClPage->ShowCL();
}

void UPDWCheatManager::PDW_TeleportToShores()
{
	Teleport_Internal(FGameplayTag::RequestGameplayTag("ID.Teleport.Shores"));
}

void UPDWCheatManager::PDW_TeleportToJungle()
{
	Teleport_Internal(FGameplayTag::RequestGameplayTag("ID.Teleport.Jungle"));
}

void UPDWCheatManager::PDW_TeleportToDinoPlains()
{
	Teleport_Internal(FGameplayTag::RequestGameplayTag("ID.Teleport.DinoPlains"));
}

void UPDWCheatManager::PDW_TeleportToVolcano()
{
	Teleport_Internal(FGameplayTag::RequestGameplayTag("ID.Teleport.Volcano"));
}

void UPDWCheatManager::PDW_TeleportToTutorial()
{
	Teleport_Internal(FGameplayTag::RequestGameplayTag("Teleport.Tutorial"));
}


void UPDWCheatManager::PDW_CheckP2Spawns()
{
	auto P1 = UPDWGameplayFunctionLibrary::GetNebulaPlayerControllerOne(GetWorld());
	if (P1 && P1->GetPawn())
	{
		for (int32 i = 0; i < 250; ++i)
		{
			FVector Origin = P1->GetPawn()->GetActorLocation();
			bool bIsValid = UPDWGameplayFunctionLibrary::IsPointInNavMesh(GetWorld(), Origin, 100.0f);
			if (bIsValid)
			{
				FVector TryLoc = UPDWGameplayFunctionLibrary::GetValidSpawnPointNear(GetWorld(), P1->GetPawn()->GetActorLocation(), 500.f * (i / 50));
				bool bFree = UPDWGameplayFunctionLibrary::IsSpawnLocationClear(GetWorld(), TryLoc, 200.f, nullptr, true);
#if WITH_EDITOR
				DrawDebugSphere(GetWorld(), TryLoc,  200.f, 16, !bFree ? FColor::Red : FColor::Green, false, 5.0f);
#endif
			}
		}
	}
}

void UPDWCheatManager::PDW_ToggleMultiplayer()
{
	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	UPDWGameInstance* GI = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameMode)
	{
		if (GI->GetIsMultiPlayerOn())
		{
			GameMode->RemovePlayer2();
		}
		else
		{
			GameMode->CreatePlayer2(1);
		}
	}
}

void UPDWCheatManager::PDW_MultipleToggleMultiplayer()
{
	if (PlayerToggleTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerToggleTimerHandle);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			PlayerToggleTimerHandle,
			this,
			&UPDWCheatManager::PDW_ToggleMultiplayer,
			1.0f,   // 1 second
			true    // loop
		);
	}
}

void UPDWCheatManager::PDW_SetNPCActivity(bool IsActive)
{
	TArray<AActor*> NPCSplines = {};
	UGameplayStatics::GetAllActorsOfClass(this, APDWMovingNPCActorsSpawner::StaticClass(), NPCSplines);

	for (AActor* Spline : NPCSplines)
	{
		Spline->SetActorTickEnabled(IsActive);
	}
}

void UPDWCheatManager::PDW_DestroyNPCs()
{
	TArray<AActor*> NPCSplines = {};
	UGameplayStatics::GetAllActorsOfClass(this, APDWMovingNPCActorsSpawner::StaticClass(), NPCSplines);

	for (int32 i = NPCSplines.Num() - 1; i >= 0; i--)
	{
		TArray<AActor*> NPCS = {};
		NPCSplines[i]->GetAttachedActors(NPCS);
		for (int32 j = NPCS.Num() - 1; j >= 0; j--)
		{
			NPCS[j]->Destroy();
		}
		NPCSplines[i]->Destroy();
	}
}

void UPDWCheatManager::EnableFreeCamera()
{
	ANebulaFlowBaseGameMode* GameMode = Cast<ANebulaFlowBaseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		return;
	}
	GameMode->EnableFreeCamera();
}

void UPDWCheatManager::DisableFreeCamera()
{
	ANebulaFlowBaseGameMode* GameMode = Cast<ANebulaFlowBaseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		return;
	}
	GameMode->DisableFreeCamera();
}

void UPDWCheatManager::PDW_ReplayMinigame(FString _MinigameTag, FString _QuestTag)
{
	FGameplayTag MinigameTag = BuildFullTag(_MinigameTag, MinigameTagPrefix);
	FGameplayTag QuestTag = BuildFullTag(_QuestTag, QuestTagPrefix);

	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		FGameplayTagContainer TagContainer;

		TagContainer.AddTagFast(MinigameTag);
		TagContainer.AddTagFast(QuestTag);
		GameMode->ReplayMinigame(TagContainer);
	}
}

void UPDWCheatManager::Teleport_Internal(FGameplayTag inLocationTag)
{
	APDWPlayerController* Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	TArray<AActor*> ActorToTP;
	if (Controller)
	{
		if (Controller->GetIsOnVehicle())
		{
			ActorToTP.Add(Controller->GetVehicleInstance());
		}
		else
		{
			ActorToTP.Add(Controller->GetPawn());
		}
	}
	UWPHelperSubsystem::Get(this)->TeleportToLocation(inLocationTag, ActorToTP);
	UPDWDataFunctionLibrary::SetPlayerAreaIdByTeleportId(this, inLocationTag);
}

void UPDWCheatManager::ReachMinigameLocation(FGameplayTagContainer MinigameTag, float TeleportHeight)
{
	FVector MinigameLocation = FVector::ZeroVector;
	UQuestSettings* QuestSettings = UQuestSettings::Get();
	UPDWQuestConfig* QuestConfig = Cast<UPDWQuestConfig>(QuestSettings->QuestConfig.TryLoad());
	for (FPDWQuestTargetLocs& TargetLoc : QuestConfig->TargetLocations)
	{
		if (TargetLoc.TagContainer.HasAllExact(MinigameTag))
		{
			TArray<FVector> LocationsArray = {};
			TargetLoc.Locations.GenerateValueArray(LocationsArray);
			MinigameLocation = LocationsArray.Num() > 0 ? LocationsArray[0] : FVector::ZeroVector;
		}
	}

	if (MinigameLocation != FVector::ZeroVector)
	{
		APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
		if (!PC)
			return;

		UWPHelperSubsystem* WPHelperSubsystem = UWPHelperSubsystem::Get(this);
		if (WPHelperSubsystem)
		{
			TArray<AActor*>ActorsToTeleport = { PC->GetPawn() };
			WPHelperSubsystem->Teleport(ActorsToTeleport, {FTransform(FRotator(), FVector(MinigameLocation.X, MinigameLocation.Y, MinigameLocation.Z + TeleportHeight), FVector::OneVector)});
		}
	}
}


void UPDWCheatManager::ResumeQuestFlow(UPDWFlowNode_Quest* QuestNode, TArray<UFlowNode*> NodesToSkip)
{
	if (QuestNode)
	{
		if (NodesToSkip.Num() > 0)
		{
			for (int32 i = NodesToSkip.Num() - 1; i >= 0; i--)
			{
				NodesToSkip[i]->SetCheatMode(true);

				if (!NodesToSkip[i]->IsA(UPDWFlowNode_Quest::StaticClass()))
				{
					NodesToSkip[i]->ForceSignalMode(EFlowSignalMode::PassThrough);
				}
				else
				{
					NodesToSkip.RemoveAt(i);
				}
			}

			NodesToSkip[0]->OnPassThrough_Implementation();
		}
	}
}

void UPDWCheatManager::ClearBreadcrumbs()
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC)
		return;

	PC->GetBreadCrumbsComponent()->ClearFakeTargets();
	PC->GetBreadCrumbsComponent()->ClearBreadCrumbTargets();

	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld()))
	{
		APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
		if(!PC2) return;
		PC2->GetBreadCrumbsComponent()->ClearFakeTargets();
		PC2->GetBreadCrumbsComponent()->ClearBreadCrumbTargets();
	}
}

FGameplayTag UPDWCheatManager::BuildFullTag(FString CurrentTag, FGameplayTag& Prefix)
{
	FString PrefixString = Prefix.ToString();
	FName FullName = FName(PrefixString.Append("." + CurrentTag));
	return UGameplayTagsManager::Get().RequestGameplayTag(FullName, false);
}
