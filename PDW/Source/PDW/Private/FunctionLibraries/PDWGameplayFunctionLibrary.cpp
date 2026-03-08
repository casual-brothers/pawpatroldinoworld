// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWGameplayTagSettings.h"
#include "PDWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FlowAsset.h"
#include "Gameplay/FLOW/PDWFlowNode_ChangeState.h"
#include "GameplayTagContainer.h"
#include "SmartObjectSubsystem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "LevelSequencePlayer.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowUIManager.h"
#include "Managers/PDWUIManager.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Data/PDWPersistentUser.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "NavigationSystem.h"
#include "Engine/OverlapResult.h"
#include "Containers/Set.h"
#include "InputAction.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Managers/NebulaFlowCommandManager.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "EnhancedInputSubsystems.h"
#include "FlowComponent.h"
#include "FlowSubsystem.h"
#include "Gameplay/FLOW/PDWFlowNode_Quest.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Components/ForceFeedbackComponent.h"
#include "QuestSettings.h"
#include "Managers/PDWAchievementManager.h"

APDWGameplayGameMode* UPDWGameplayFunctionLibrary::GetPDWGameplayGameMode(const UObject* WorldContextObject)
{
	return APDWGameplayGameMode::Get(WorldContextObject);
}

APDWPlayerController* UPDWGameplayFunctionLibrary::GetPlayerControllerOne(const UObject* WorldContextObject)
{
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(WorldContextObject);
	if (!ensureMsgf(IsValid(GM), TEXT("PDWGameplayFunctionLibrary Can't retrive GameMode")))
		return nullptr;
	return GM->GetPlayerControllerOne();
}

APDWPlayerController* UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(const UObject* WorldContextObject)
{
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(WorldContextObject);
	if (!ensureMsgf(IsValid(GM), TEXT("PDWGameplayFunctionLibrary Can't retrive GameMode")))
		return nullptr;
	return GM->GetPlayerControllerTwo();
}

ANebulaFlowPlayerController* UPDWGameplayFunctionLibrary::GetNebulaPlayerControllerOne(const UObject* WorldContextObject)
{
	ANebulaFlowPlayerController* PlayerController = Cast<ANebulaFlowPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	return PlayerController;
}

ANebulaFlowPlayerController* UPDWGameplayFunctionLibrary::GetNebulaPlayerControllerTwo(const UObject* WorldContextObject)
{
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(WorldContextObject);
	if (!ensureMsgf(IsValid(GM), TEXT("PDWGameplayFunctionLibrary Can't retrive GameMode")))
		return nullptr;
	return GM->GetPlayerControllerTwo();
}

APDWCharacter* UPDWGameplayFunctionLibrary::GetPlayerOne(const UObject* WorldContextObject)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(WorldContextObject);
	if (!ensureMsgf(IsValid(PC), TEXT("PDWGameplayFunctionLibrary Can't retrive PC")))
		return nullptr;
	return Cast<APDWCharacter>(PC->GetPawn());
}

APDWCharacter* UPDWGameplayFunctionLibrary::GetPlayerTwo(const UObject* WorldContextObject)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(WorldContextObject);
	if (!ensureMsgf(IsValid(PC), TEXT("PDWGameplayFunctionLibrary Can't retrive PC2")))
		return nullptr;
	return Cast<APDWCharacter>(PC->GetPawn());
}

TArray<APDWCharacter*> UPDWGameplayFunctionLibrary::GetPlayers(const UObject* WorldContextObject)
{
	TArray<APDWCharacter*> Result = {};

	if (APDWCharacter* P1Character = UPDWGameplayFunctionLibrary::GetPlayerOne(WorldContextObject))
	{
		Result.Add(P1Character);
	}
	if (APDWCharacter* P2Character = UPDWGameplayFunctionLibrary::GetPlayerTwo(WorldContextObject))
	{
		Result.Add(P2Character);
	}
	
	return Result;
}

APDWPlayerState* UPDWGameplayFunctionLibrary::GetPlayerStateOne(const UObject* WorldContextObject)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(WorldContextObject);
	if (!ensureMsgf(IsValid(PC), TEXT("PDWGameplayFunctionLibrary Can't retrive PC")))
		return nullptr;
	return PC->GetPDWPlayerState();
}

APDWPlayerState* UPDWGameplayFunctionLibrary::GetPlayerStateTwo(const UObject* WorldContextObject)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(WorldContextObject);
	if (!ensureMsgf(IsValid(PC), TEXT("PDWGameplayFunctionLibrary Can't retrive PC2")))
		return nullptr;
	return PC->GetPDWPlayerState();
}

APDWCharacter* UPDWGameplayFunctionLibrary::GetPlayerByIndex(const UObject* WorldContextObject, const int32 Index)
{
	APDWCharacter* Result = nullptr;	
	switch (Index)
    {
        case 0:
            Result = GetPlayerOne(WorldContextObject);
            break;
        case 1:
            Result = GetPlayerTwo(WorldContextObject);
            break;
    }
	return Result;
}

const FTransform UPDWGameplayFunctionLibrary::GetSocketTransformFromSpecificMesh(const AActor* inOwner, const FName& inSocketName, const FName& inMeshTag /*= NAME_None*/)
{
	if (!IsValid(inOwner))
		return FTransform::Identity;

	if (USkeletalMeshComponent* MeshComp = inOwner->FindComponentByTag<USkeletalMeshComponent>(inMeshTag))
	{
		if (MeshComp->DoesSocketExist(inSocketName))
		{
			return MeshComp->GetSocketTransform(inSocketName);
		}
	}

	return FTransform::Identity;
}

const bool UPDWGameplayFunctionLibrary::IsSecondPlayer(const APlayerController* PlayerControllerToVerify)
{
	if (IsValid(PlayerControllerToVerify) && PlayerControllerToVerify->GetWorld())
	{
		return PlayerControllerToVerify->GetWorld()->GetFirstPlayerController() != PlayerControllerToVerify;
	}
	return false;
}

const bool UPDWGameplayFunctionLibrary::IsSecondPlayerByActor(const UObject* WorldContextObject, const AActor* ActorToVerify)
{
	if (APDWCharacter* P2 = GetPlayerTwo(WorldContextObject))
	{
		return ActorToVerify == P2;
	}
	return false;
}

const bool UPDWGameplayFunctionLibrary::IsMultiplayerOn(const UObject* WorldContextObject)
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (GI)
	{
		UPDWGameInstance* GPDW = Cast<UPDWGameInstance>(GI);
		if (GPDW)
		{
			return GPDW->GetIsMultiPlayerOn();
		}
	}

	return false;
}

const void UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(UObject* WorldContextObject, TMap<TObjectPtr<UDataLayerAsset>, bool> DataLayers, bool SaveIt)
{
	if (UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(WorldContextObject))
	{
		for (TPair<UDataLayerAsset*, bool> DL : DataLayers)
		{
			DataLayerManager->SetDataLayerRuntimeState(DL.Key, DL.Value ? EDataLayerRuntimeState::Activated : EDataLayerRuntimeState::Unloaded, false);

			if (SaveIt)
			{
				UPDWDataFunctionLibrary::SetDataLayerState(WorldContextObject, DL.Key, DL.Value);
			}
		}
	}
}

const void UPDWGameplayFunctionLibrary::BP_SetDataLayerRuntimeState(UObject* WorldContextObject, TMap<UDataLayerAsset*, bool> DataLayers, bool SaveIt /*= true*/)
{
	if (UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(WorldContextObject))
	{
		for (TPair<UDataLayerAsset*, bool> DL : DataLayers)
		{
			DataLayerManager->SetDataLayerRuntimeState(DL.Key, DL.Value ? EDataLayerRuntimeState::Activated : EDataLayerRuntimeState::Unloaded, false);

			if (SaveIt)
			{
				UPDWDataFunctionLibrary::SetDataLayerState(WorldContextObject, DL.Key, DL.Value);
			}
		}
	}	
}

bool UPDWGameplayFunctionLibrary::IsNextNodeChangeState(UFlowAsset* FlowAsset)
{
	if (!FlowAsset)
		return false;

	TArray<UFlowNode*> ActiveNodes = FlowAsset->GetActiveNodes();
	if (ActiveNodes.IsValidIndex(0) && ActiveNodes[ActiveNodes.Num() - 1])
	{
		TArray<UFlowNode*> AllNodes = FlowAsset->GetNodesInExecutionOrder(ActiveNodes[ActiveNodes.Num() - 1], UFlowNode::StaticClass());
		if (AllNodes.IsValidIndex(1) && AllNodes[1])
		{
			return AllNodes[1]->IsA(UPDWFlowNode_ChangeState::StaticClass());
		}
	}

	return false;
}

void UPDWGameplayFunctionLibrary::TriggerQuestTalkingEvent(UObject* WorldContextObject)
{
	UPDWEventSubsytem::Get(WorldContextObject)->OnQuestTalkingEvent();
}

void UPDWGameplayFunctionLibrary::SendSmartObjectEvent(const UObject* WorldContextObject, const FSmartObjectSlotHandle SlotHandle, const FGameplayTag EventTag, AActor* Instigator)  //const FGameplayTag EventTag, const FConstStructView Payload = FConstStructView()
{
	if (USmartObjectSubsystem* SmartObjectSubsystem = WorldContextObject->GetWorld()->GetSubsystem<USmartObjectSubsystem>())
	{
		const FSmartObjectActorUserData ActorUserData(Instigator);
		SmartObjectSubsystem->SendSlotEvent(SlotHandle,EventTag,FConstStructView::Make(ActorUserData));
	}
}

void UPDWGameplayFunctionLibrary::AddItemsToInventory(UObject* WorldContextObject, TMap<FGameplayTag, int32> Items, bool Notify)
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
	UPDWGameInstance* GPDW = Cast<UPDWGameInstance>(GI);
	if (!GPDW)	
		return;

	for(TPair<FGameplayTag, int32> Item : Items)
	{
		UPDWDataFunctionLibrary::AddItemToInventory(WorldContextObject, Item.Key, Item.Value);

		if (Notify)
		{
			if (UPDWEventSubsytem* EventSubsystem = GPDW->GetSubsystem<UPDWEventSubsytem>())
			{
				EventSubsystem->OnItemCollectedEvent(Item.Key);
			}
		}		
	}
}

ULevelSequencePlayer* UPDWGameplayFunctionLibrary::PDWCreateLevelSequence(UObject* WorldContextObject, ULevelSequence* InLevelSequence, FMovieSceneSequencePlaybackSettings Settings, ALevelSequenceActor*& OutActor)
{
	return ULevelSequencePlayer::CreateLevelSequencePlayer(WorldContextObject,InLevelSequence,Settings,OutActor);
}

UPDWUIManager* UPDWGameplayFunctionLibrary::GetPDWUIManager(UObject* WorldContextObject)
{
	UNebulaFlowUIManager* NebulaManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (NebulaManager)
	{
		if (UPDWUIManager* UIManager = Cast<UPDWUIManager>(NebulaManager))
		{
			return UIManager;
		}
	}
	return nullptr;
}

UTextureRenderTarget2D* UPDWGameplayFunctionLibrary::PDWCreateRenderTarget2D(UObject* WorldContextObject, int32 Width, int32 Height, ETextureRenderTargetFormat Format, FLinearColor ClearColor, bool bAutoGenerateMipMaps, bool bSupportUAVs)
{
	return UKismetRenderingLibrary::CreateRenderTarget2D(WorldContextObject,Width,Height,Format,ClearColor,bAutoGenerateMipMaps,bSupportUAVs);
}

UPDWPersistentUser* UPDWGameplayFunctionLibrary::GetPDWPersistentUser(UObject* WorldContextObject)
{
	UNebulaFlowLocalPlayer* PlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject);
	if (PlayerOwner)
	{
#if WITH_EDITOR
		if (!Cast<UPDWPersistentUser>(PlayerOwner->GetPersistentUser()))
		{
			PlayerOwner->LoadPersistentUser();
		}
#endif
		return Cast<UPDWPersistentUser>(PlayerOwner->GetPersistentUser());
	}

	return nullptr;
}

FGameProgressionSlot& UPDWGameplayFunctionLibrary::GetCurrentProgressionSlot(UObject* WorldContextObject, bool& WasSuccessful)
{
	WasSuccessful = false;

	UPDWPersistentUser* PersistentUser = GetPDWPersistentUser(WorldContextObject);
	return PersistentUser->GetCurrentUsedProgressionSlot(WasSuccessful);
}

void UPDWGameplayFunctionLibrary::TriggerSequenceDialogue(UObject* WorldContextObject)
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(WorldContextObject,UPDWGameSettings::GetUIActionDialogueFromSequence().ToString(),"");
}

void UPDWGameplayFunctionLibrary::StartCameraShake(APlayerController* PlayerController, TSubclassOf<UCameraShakeBase> ShakeClass)
{
	if(!PlayerController || !ShakeClass || (!UGameOptionsFunctionLibrary::GetCameraShakeEnabled()))
		return;

	PlayerController->ClientStartCameraShake(ShakeClass);
}

void UPDWGameplayFunctionLibrary::StopCameraShake(APlayerController* PlayerController, TSubclassOf<UCameraShakeBase> ShakeClass, bool bImmediately /*= true*/)
{
	if(!PlayerController || !ShakeClass || (!UGameOptionsFunctionLibrary::GetCameraShakeEnabled()))
		return;

	PlayerController->ClientStopCameraShake(ShakeClass, bImmediately);
}

void UPDWGameplayFunctionLibrary::PlayForceFeedback(TArray<APDWPlayerController*> PlayerControllers, FGameplayTag ForceFeedbackTag, FName Tag, bool bLooping, bool bIgnoreTimeDilation, bool bPlayWhilePaused)
{
	if(!UGameOptionsFunctionLibrary::GetGamepadRumbleEnabled()) return;	

	UForceFeedbackEffect* Effect = PlayerControllers[0]->GetVibrationsByTag(ForceFeedbackTag);
	if (!Effect) return;

	FForceFeedbackParameters Params;
	Params.Tag = ForceFeedbackTag.GetTagName();
	Params.bLooping = bLooping;
	Params.bIgnoreTimeDilation = bIgnoreTimeDilation;
	Params.bPlayWhilePaused = bPlayWhilePaused;

	for (APDWPlayerController* PlayerController : PlayerControllers)
	{
		if (!PlayerController || !PlayerController->GetForceFeedbackComponent()) continue;
	
		UForceFeedbackComponent* ForceFeedbackComp = PlayerController->GetForceFeedbackComponent();
		ForceFeedbackComp->SetIntensityMultiplier(UGameOptionsFunctionLibrary::GetGamepadRumbleIntensity());
		ForceFeedbackComp->SetForceFeedbackEffect(Effect);
		ForceFeedbackComp->bLooping = bLooping;
		ForceFeedbackComp->bIgnoreTimeDilation = bIgnoreTimeDilation;
		//PlayWhilePaused?
		ForceFeedbackComp->Play();
	}
	
}

void UPDWGameplayFunctionLibrary::PlayForceFeedbackBothPlayers(UObject* WorldContextObject, FGameplayTag ForceFeedbackTag, FName Tag /*= NAME_None*/, bool bLooping /*= false*/, bool bIgnoreTimeDilation /*= false*/, bool bPlayWhilePaused /*= false*/)
{
	TArray<APDWPlayerController*> PlayerControllers = {};
	PlayerControllers.Add(GetPlayerControllerOne(WorldContextObject));
	PlayerControllers.Add(GetPlayerControllerTwo(WorldContextObject));
	PlayForceFeedback(PlayerControllers,ForceFeedbackTag,Tag,bLooping,bIgnoreTimeDilation,bPlayWhilePaused);
}

void UPDWGameplayFunctionLibrary::StopForceFeedback(TArray<APDWPlayerController*> PlayerControllers)
{
	if(!UGameOptionsFunctionLibrary::GetGamepadRumbleEnabled())
		return;

	for (APDWPlayerController* PlayerController : PlayerControllers)
	{
		if (!PlayerController || !PlayerController->GetForceFeedbackComponent()) continue;

		PlayerController->GetForceFeedbackComponent()->Stop();
	}
}

void UPDWGameplayFunctionLibrary::StopForceFeedbackBothPlayers(UObject* WorldContextObject)
{
	TArray<APDWPlayerController*> PlayerControllers = {};
	PlayerControllers.Add(GetPlayerControllerOne(WorldContextObject));
	PlayerControllers.Add(GetPlayerControllerTwo(WorldContextObject));
	StopForceFeedback(PlayerControllers);
}

bool UPDWGameplayFunctionLibrary::GetRandomSpawnLocationNear(UWorld* World, FVector& Location, AActor* ActorToIgnore, bool bP2)
{
	if (IsPointInNavMesh(World, Location, 100.0f))
	{
		if (IsSpawnLocationClear(World, Location, 200.f, ActorToIgnore, bP2))
		{
			return true;
		}
		for (int32 i = 0; i < 250; ++i)
		{
			FVector TryLoc = GetValidSpawnPointNear(World, Location, 500.f * (1 + (i / 50)));
			if (IsSpawnLocationClear(World, TryLoc, 200.f, ActorToIgnore))
			{
				Location = TryLoc + FVector::UpVector * 100.0f;
				return true;
			}
		}
	}
	return false;
}


bool UPDWGameplayFunctionLibrary::IsPointInNavMesh(UWorld* World, const FVector& Point, float SearchRadius)
{
    if (!World) return false;

    FNavLocation NavLocation;
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavSys) return false;

    // Try to project the point onto the NavMesh
    bool bOnNavMesh = NavSys->ProjectPointToNavigation(
        Point,
        NavLocation,
		FVector(SearchRadius, SearchRadius, SearchRadius + 300.0f) // search extents
    );

    return bOnNavMesh;
}

FVector UPDWGameplayFunctionLibrary::GetValidSpawnPointNear(UWorld* World, FVector Origin, float Radius)
{
    if (!World)
        return Origin;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavSys)
        return Origin;

    FNavLocation ResultLocation;
    bool bFound = NavSys->GetRandomReachablePointInRadius(Origin, Radius, ResultLocation);

    if (bFound)
    {
        return ResultLocation.Location;
    }
    else
    {
        // Fallback: return the origin if no nav point found
        return Origin;
    }
}

bool UPDWGameplayFunctionLibrary::IsSpawnLocationClear(UWorld* World, FVector Location, float Radius, AActor* ActorToIgnore, bool bP2)
{
    if (!World)
        return false;

	// --- Object types we care about (dynamic things only) ---
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel3); //Pup
    ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel14); //Dino
	if(bP2)
	{
		ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel13); //NoSpawnArea
	}

    // --- Actors/components to ignore ---
    FCollisionQueryParams QueryParams;
    if (ActorToIgnore)
         QueryParams.AddIgnoredActor(ActorToIgnore);

    // --- Collision shape for our test ---
    const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);

    // --- Perform overlap test (checks current penetrations, not movement) ---
    TArray<FOverlapResult> Overlaps;
    bool bHasOverlap = World->OverlapMultiByObjectType(
        Overlaps,
        Location,
        FQuat::Identity,
        ObjectQueryParams,
        CollisionShape,
        QueryParams
    );

    bool bWouldCollideWithPawn = false;

    if (bHasOverlap)
    {
        for (const FOverlapResult& Result : Overlaps)
        {
            if (Result.Component.IsValid())
            {
                ECollisionResponse Response = Result.Component->GetCollisionResponseToChannel(ECC_GameTraceChannel3);
				ECollisionResponse Response2 = Result.Component->GetCollisionResponseToChannel(ECC_Vehicle);
				ECollisionResponse Response3 = Result.Component->GetCollisionResponseToChannel(ECC_GameTraceChannel13);
				ECollisionResponse Response4 = Result.Component->GetCollisionResponseToChannel(ECC_GameTraceChannel14);
				if (Response == ECR_Block || Response2 == ECR_Block || Response3 == ECR_Block || Response4 == ECR_Block)
				{
                    bWouldCollideWithPawn = true;

#if WITH_EDITOR
					UE_LOG(LogClass, Error, TEXT("Would collide with: %s"), *Result.GetActor()->GetName());
#endif
                }
            }
        }
    }

#if WITH_EDITOR
    //DrawDebugSphere(World, Location, Radius, 16, bWouldCollideWithPawn ? FColor::Red : FColor::Green, false, 5.0f);
#endif

    // If false → safe to spawn
    return !bWouldCollideWithPawn;
}

void UPDWGameplayFunctionLibrary::AddDisabledInputAction(UObject* WorldContextObject, FName InputAction)
{
	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode)
	{
		GameMode->DisabledInputActions.Add(InputAction);
		UPDWDataFunctionLibrary::AddDisabledInput(WorldContextObject,InputAction);
	}
}

void UPDWGameplayFunctionLibrary::RemoveDisabledInputAction(UObject* WorldContextObject, FName InputAction)
{
	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode)
	{
		GameMode->DisabledInputActions.Remove(InputAction);
		UPDWDataFunctionLibrary::RemoveDisabledInput(WorldContextObject,InputAction);
	}
}

bool UPDWGameplayFunctionLibrary::IsInputActionDisabled(UObject* WorldContextObject, UInputAction* InputAction)
{
    if (!InputAction)
    {
        return false;
	}

	const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!GameMode)
	{
		return false;
	}
	return GameMode->DisabledInputActions.Contains(InputAction->GetFName());
}

void UPDWGameplayFunctionLibrary::SetVehiclesPhysiscs(UObject* WorldContextObject, bool IsActive)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(WorldContextObject);
	if (PC)
	{
		if (PC->GetVehicleInstance())
		{
			PC->GetVehicleInstance()->SetPhysActiveCarBP(IsActive);
		}
	}
	APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(WorldContextObject);
	if (PC2)
	{
		if (PC2->GetVehicleInstance())
		{
			PC2->GetVehicleInstance()->SetPhysActiveCarBP(IsActive);
		}
	}
}

void UPDWGameplayFunctionLibrary::ApplyQualitySettings(UObject* WorldContextObject, const FString SectionName)
{
	UNebulaFlowCommandManager* Command = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowCommandManager(WorldContextObject);
	Command->ApplyCommands(*SectionName, *GDeviceProfilesIni);
}

void UPDWGameplayFunctionLibrary::RestoreQualitySettings(UObject* WorldContextObject, const FString SectionName)
{
	UNebulaFlowCommandManager* Command = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowCommandManager(WorldContextObject);
	Command->RestoreCommands(*SectionName);
}

TArray<FKey> UPDWGameplayFunctionLibrary::FindAllKeysForAction(UObject* WorldContextObject, UInputAction* Action)
{
    TArray<FKey> BoundKeys = TArray<FKey>();
    
    APlayerController* PC = WorldContextObject->GetWorld()->GetFirstPlayerController();
    if (!Action || !PC)
    {
        return BoundKeys;
    }

    ULocalPlayer* LP = PC->GetLocalPlayer();
    if (!LP)
    {
        return BoundKeys;
    }

	const UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    if (!Subsystem)
    {
        return BoundKeys;
    }
    
    BoundKeys = Subsystem->QueryKeysMappedToAction(Action);

    return BoundKeys;
}

TArray<FQuestData> UPDWGameplayFunctionLibrary::GetAreaQuests(UObject* WorldContextObject, FGameplayTag AreaID)
{
	TArray<FQuestData> ToReturn = {};
	TArray<FQuestData> CompletedQuests = UPDWDataFunctionLibrary::GetCompletedQuests(WorldContextObject);

	//TODO save this info with bluetility collectquesttarget
	TObjectPtr<UFlowComponent> RootFlow = GetQuestManagerComp(WorldContextObject);
	if(!RootFlow) return TArray<FQuestData>();

	UFlowAsset* RootFlowAsset = RootFlow->GetRootFlowInstance();
	if(!RootFlowAsset) return TArray<FQuestData>();

	TArray<UFlowNode*> AllQuestNodes = RootFlowAsset->GetNodesInExecutionOrder(RootFlowAsset->GetDefaultEntryNode(), UPDWFlowNode_Quest::StaticClass());
	for(UFlowNode* FlowNode : AllQuestNodes)
	{
		UPDWFlowNode_Quest* QuestNode = Cast<UPDWFlowNode_Quest>(FlowNode);
		UFlowAsset* SubGraphAsset = QuestNode->GetSubGraphAsset().LoadSynchronous();
		UBaseFlowQuest* QuestSubGraphAsset = Cast<UBaseFlowQuest>(SubGraphAsset);

		if(!QuestSubGraphAsset || QuestSubGraphAsset->GetQuestArea() != AreaID) continue;

		FQuestData QuestData = {};
		QuestData.QuestID = QuestSubGraphAsset->GetQuestID();
		QuestData.AreaID = QuestSubGraphAsset->GetQuestArea();
		for(FQuestData& CompletedQuest : CompletedQuests)
		{
			if (CompletedQuest.QuestID == QuestSubGraphAsset->GetQuestID())
			{
				QuestData.IsCompleted = true;
			}
		}

		ToReturn.Add(QuestData);
	}

	return ToReturn;
}

UFlowComponent* UPDWGameplayFunctionLibrary::GetQuestManagerComp(UObject* WorldContextObject)
{
	if (UFlowSubsystem* FlowSubsystem = UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UFlowSubsystem>())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTagFast(UGameplayTagsManager::Get().RequestGameplayTag("Flow.Root", false));

		for (const TWeakObjectPtr<UFlowComponent>& Component : FlowSubsystem->GetComponents<UFlowComponent>(TagContainer, EGameplayContainerMatchType::All, true))
		{
			return Component.Get();
		}
	}

	return nullptr;
}

TArray<FPDWMinigameInfo> UPDWGameplayFunctionLibrary::GetPlayedMinigames(UObject* WorldContextObject)
{
	TArray<FPDWMinigameInfo> PlayedMinigames =  UPDWDataFunctionLibrary::GetPlayedMinigamesInfo(WorldContextObject);
	UQuestSettings* QuestSettings = UQuestSettings::Get();
	UPDWQuestConfig* QuestConfig = Cast<UPDWQuestConfig>(QuestSettings->QuestConfig.TryLoad());

	for(FPDWMinigameInfo& Minigame : PlayedMinigames)
	{
		for (FPDWQuestTargetLocs& TargetLoc : QuestConfig->TargetLocations)
		{
			if (Minigame.IdentityTags.HasAllExact(TargetLoc.TagContainer))
			{
				Minigame.Name = TargetLoc.Name;
				Minigame.AllowedPup = TargetLoc.AllowedPup;
			}
		}
	}

	return PlayedMinigames;
}

UPDWAchievementManager* UPDWGameplayFunctionLibrary::GetAchievementManager(UObject* WorldContextObject)
{
	return Cast<UPDWAchievementManager>(UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAchievementManager(WorldContextObject));
}
