// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "FlowComponent.h"
#include "UI/Widgets/CLDebugWidget.h"
#include "FlowSubsystem.h"
#include "FlowAsset.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "PDWGameInstance.h"
#include "Data/PDWGameSettings.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Data/FlowDeveloperSettings.h"
#include "Managers/QuestSubsystem.h"
#include "Data/PDWPupConfigurationAsset.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/FLOW/PDWFlowNode_ChangeState.h"
#include "Nodes/Graph/FlowNode_SubGraph.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "Managers/PDWEventSubsytem.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Data/PDWPersistentUser.h"
#include "Data/PDWPlayerState.h"
#include "WPHelperSubsystem.h"
#include "NavigationSystem.h"
#include "QuestSettings.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Managers/PDWUIManager.h"

void APDWGameplayGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void APDWGameplayGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestSubsystem->InitGameplay();
	}

	BeforeSaveHandle = FNebulaFlowCoreDelegates::OnBeforSave.AddUObject(this, &APDWGameplayGameMode::OnBeforeSave);

	#if !UE_BUILD_SHIPPING
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (!LevelName.Equals(FString("WorldMap")))
	{
		IsTestMap = true;
	}
	#endif

	if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		FlowSubsystem->OnGameLoaded(UPDWDataFunctionLibrary::GetFlowSaveGame(GetWorld()));
	}

	SwitcherWaitForAction = true;

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnInitGameEnter.AddDynamic(this, &APDWGameplayGameMode::OnInitGameStateEnter);
	}

	//Init PaleoCenter
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->Init();
	}

	if (!UPDWDataFunctionLibrary::GetIsNewGame(this))
	{
		//LOAD LOGICS
		AActor* PlayerStart = FindPlayerStart(nullptr);
		if (PlayerStart)
		{
			if (!IsTestMap)
			{
				PlayerStart->SetActorLocation(FVector(1000000,1000000,1000000));
			}
		}

		DisabledInputActions = UPDWDataFunctionLibrary::GetDisabledInputs(this);
		TMap<TObjectPtr<UDataLayerAsset>, bool> DL = UPDWDataFunctionLibrary::GetDataLayersState(this);
		UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL, false);
	}

	//Spawn Helpers
	if (FSMHelperClass)
	{
		FSMHelper = NewObject<UPDWFsmHelper>(this, FSMHelperClass, FName("FSMHelper"));
	}

	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		UIManager->ShowTransitionImmediatly();
	}
}

void APDWGameplayGameMode::StartPlay()
{
	Super::StartPlay();

	if (CLWidgetClass)
	{
		ClPage = CreateWidget<UCLDebugWidget>(UGameplayStatics::GetGameInstance(this), CLWidgetClass);
		ClPage->AddToViewport(100);
	}

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->OnStartPlay();
	}

	UNebulaFlowLocalPlayer* PlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	if(PlayerOwner)
	{		
		if (APDWPlayerController* PDWPlayerController = Cast<APDWPlayerController>(PlayerOwner->GetPlayerController(GetWorld())))
		{
			PC_One = PDWPlayerController;

			if (IsTestMap)
			{
				OnTeleportCompletedEvent();
				return;
			}
			TArray<AActor*> Player = {PDWPlayerController->GetPupInstance()};
			const FTransform PlayerPosition = UPDWDataFunctionLibrary::GetIsNewGame(this) ?  FindPlayerStart(nullptr)->GetTransform() : UPDWDataFunctionLibrary::GetPlayerPosition(this);

			UWPHelperSubsystem::Get(this)->Teleport(Player, {PlayerPosition});
			UWPHelperSubsystem::Get(this)->OnTeleportCompleted.AddUniqueDynamic(this,&ThisClass::OnTeleportCompletedEvent);
		}
	}

	//UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetSwitcherTag().ToString(), "");
	//Start world logics and timers
	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &APDWGameplayGameMode::OnGameReady);
}

void APDWGameplayGameMode::RestartPlayer(AController* NewPlayer)
{
    UWorld* World = GetWorld();
    if (!World || !NewPlayer || !PC_One)
    {
        Super::RestartPlayer(NewPlayer);
        return;
    }

    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    if (PC_One && PC_One->GetPawn())
    {
		SpawnRotation = PC_One->GetPawn()->GetActorRotation();

		FVector Origin = PC_One->GetPawn()->GetActorLocation();
		if (UPDWGameplayFunctionLibrary::GetRandomSpawnLocationNear(World, Origin, nullptr, true))
		{
			SpawnLocation = Origin;
		}
		if (SpawnLocation == FVector::ZeroVector)
		{
			SpawnLocation = PC_One->GetPawn()->GetActorLocation() + FVector::UpVector * 200.0f;
		}
    }

    FTransform SpawnTransform(SpawnRotation, SpawnLocation);

    // Actually spawn the pawn
    APawn* NewPawn = SpawnDefaultPawnAtTransform(NewPlayer, SpawnTransform);
    if (NewPawn)
    {
        NewPlayer->SetPawn(NewPawn);
		FinishRestartPlayer(NewPlayer, SpawnRotation);
    }
    else
    {
        // Fallback to default spawn
        Super::RestartPlayer(NewPlayer);
		NewPlayer->GetPawn()->SetActorTransform(SpawnTransform);
    }
}

void APDWGameplayGameMode::CreatePlayer2(int32 UserIndex)
{
	if (PC_Two)
	{
		return;
	}

	PC_Two = Cast<APDWPlayerController>(UGameplayStatics::CreatePlayer(this, UserIndex));
	UPDWGameInstance* GI = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->SetMultiPlayerOn(true);

	UPDWGameplayFunctionLibrary::ApplyQualitySettings(this, "SplitScreen");

	UGameplayStatics::SetForceDisableSplitscreen(this, false);
}

void APDWGameplayGameMode::RemovePlayer2()
{
	if (!PC_Two)
	{
		return;
	}
	if (IsValid(PC_Two->GetVehicleInstance()))
	{
		PC_Two->GetVehicleInstance()->Destroy();
	}
	UGameplayStatics::RemovePlayer(PC_Two, true);
	PC_Two = nullptr;
	UPDWGameInstance* GI = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->SetMultiPlayerOn(false);
	
	UGameplayStatics::SetForceDisableSplitscreen(this, true);

	UPDWGameplayFunctionLibrary::RestoreQualitySettings(this, "SplitScreen");
}

void APDWGameplayGameMode::ReplayMinigame(FGameplayTagContainer MinigameTags)
{
	UQuestSettings* QuestSettings = UQuestSettings::Get();
	UPDWQuestConfig* QuestConfig = Cast<UPDWQuestConfig>(QuestSettings->QuestConfig.TryLoad());
	for (FPDWQuestTargetLocs& TargetLoc : QuestConfig->TargetLocations)
	{
		if (TargetLoc.TagContainer.HasAllExact(MinigameTags))
		{
			TArray<FVector> LocationsArray = {};
			TargetLoc.Locations.GenerateValueArray(LocationsArray);
			FVector MinigameLocation = LocationsArray.Num() > 0 ? LocationsArray[0] : FVector::ZeroVector;

			if (MinigameLocation != FVector::ZeroVector)
			{
				APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
				if (!PC)
					return;

				UWPHelperSubsystem* WPHelperSubsystem = UWPHelperSubsystem::Get(this);
				if (WPHelperSubsystem)
				{
					BeforeReplayMinigameLocation = PC->GetPawn()->GetActorTransform();
					APawn* MyPawn = nullptr;
					if (PC->GetIsOnVehicle())
					{
						MyPawn = PC->GetVehicleInstance();
					}
					else
					{
						MyPawn = PC->GetPupInstance();
					}
					TArray<AActor*>ActorsToTeleport = { MyPawn };
					PendingReplayMinigame = TargetLoc;
					UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), TargetLoc.MinigameLayersInfo);
					WPHelperSubsystem->OnTeleportCompleted.AddUniqueDynamic(this, &ThisClass::OnMinigameTeleportCompleted);
					WPHelperSubsystem->Teleport(ActorsToTeleport,{ FTransform(FRotator(), FVector(MinigameLocation.X, MinigameLocation.Y, MinigameLocation.Z + 100.f), FVector::OneVector)});
				}
			}
		}
	}
}

void APDWGameplayGameMode::TriggerFlowEntryPoint(const FName& EventName)
{

}

void APDWGameplayGameMode::OnTeleportCompletedEvent()
{
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this,&ThisClass::OnTeleportCompletedEvent);
	if (!UPDWDataFunctionLibrary::GetIsNewGame(this))
	{
		FVector Location = PC_One->GetPawn()->GetActorLocation();
		
		if (UPDWGameplayFunctionLibrary::GetRandomSpawnLocationNear(GetWorld(), Location, PC_One->GetPawn()))
		{
			PC_One->GetPawn()->SetActorLocation(Location);
		}
		const FGameplayTag& CurrentPup = UPDWDataFunctionLibrary::GetSelectedPup(this);
		const bool bIsOnVehicle = UPDWDataFunctionLibrary::GetIsOnVehicle(this);

		if (!bIsOnVehicle && CurrentPup == FGameplayTag::RequestGameplayTag("ID.Character.Marshall"))
		{
			PC_One->GetPupInstance()->CustomizePup(CurrentPup);
		}
		else if (CurrentPup != FGameplayTag::RequestGameplayTag("ID.Character.Marshall"))
		{
			PC_One->ChangeCharacter(CurrentPup);
		}

		if (bIsOnVehicle)
		{
			PC_One->ChangeVehicle(true);
		}
	}
	else
	{
		if(PC_One && PC_One->GetPDWPlayerState())
		{
			UPDWDataFunctionLibrary::SetSelectedPup(this, PC_One->GetPDWPlayerState()->GetCurrentPup());
		}
	}


	StartPlayReady = true;

	if (InitStateReady)
	{
		OnGameReady();
	}
}

void APDWGameplayGameMode::OnGameReady()
{
	StartPlayReady = false;
	InitStateReady = false;

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnInitGameEnter.RemoveDynamic(this, &APDWGameplayGameMode::OnInitGameStateEnter);
		EventSubsystem->OnSwitcherStateWaitForAction.AddDynamic(this, &APDWGameplayGameMode::OnSwitcherStateEnter);
	}

	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetSwitcherTag().ToString(), "");
}

void APDWGameplayGameMode::OnInitGameStateEnter()
{
	InitStateReady = true;

	if (StartPlayReady)
	{
		OnGameReady();
	}
}

void APDWGameplayGameMode::OnSwitcherStateEnter()
{
	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnSwitcherStateWaitForAction.RemoveDynamic(this, &APDWGameplayGameMode::OnSwitcherStateEnter);
	}
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		UIManager->StartTransitionOut();
	}
	UPDWGameInstance::Get(this)->OnGameReady();
	bIsGameReady = true;
	GameReady.Broadcast();

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->StartPaleoQuests(!UPDWDataFunctionLibrary::GetIsNewGame(this));
	}

	UPDWDataFunctionLibrary::SetIsNewGame(false, this);
}

void APDWGameplayGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Uninit PaleoCenter
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->Uninit();
	}

	Super::EndPlay(EndPlayReason);
}

void APDWGameplayGameMode::OnMinigameTeleportCompleted()
{
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this,&ThisClass::OnMinigameTeleportCompleted);

	if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		bool Found = false;

		for (AActor* Actor : FlowSubsystem->GetFlowActorsByTags(PendingReplayMinigame.TagContainer,EGameplayContainerMatchType::All, AActor::StaticClass(), true))
		{
			if (!Actor) continue;
			Found = true;
			ProcessInteraction(Cast<UPDWInteractionReceiverComponent>(Actor->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass())));
			break;
		}

		if (!Found)
		{
			FlowSubsystem->OnComponentRegistered.AddUniqueDynamic(this, &APDWGameplayGameMode::OnComponentRegistered);
		}
	}
}

void APDWGameplayGameMode::OnComponentRegistered(UFlowComponent* Component)
{
	if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		if(!Component || !Component->GetOwner() || !Component->IdentityTags.HasAllExact(PendingReplayMinigame.TagContainer)) return;

		FlowSubsystem->OnComponentRegistered.RemoveDynamic(this, &APDWGameplayGameMode::OnComponentRegistered);
		UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(Component->GetOwner()->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
		if (InteractionComp)
		{
			//Wait next tick, the interaction component is not ready yet
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, InteractionComp]()
				{
					ProcessInteraction(InteractionComp);
				});
		}
	}
}

void APDWGameplayGameMode::ProcessInteraction(UPDWInteractionReceiverComponent* InteractionComp)
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (!PC || !InteractionComp) return;

	AActor* MinigameOwner = InteractionComp->GetOwner();
	if(!MinigameOwner) return;

	UPDWMinigameConfigComponent* MinigameComp = Cast<UPDWMinigameConfigComponent>(MinigameOwner->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
	if (!MinigameComp) return;

	MinigameComp->bShowSuccessMessage = false;
	MinigameComp->bSkipJoinRequestMessage = true;
	MinigameComp->OnMinigameComplete.AddUniqueDynamic(this, &APDWGameplayGameMode::OnReplayMinigameComplete);
	MinigameComp->OnMinigameLeft.AddUniqueDynamic(this, &APDWGameplayGameMode::OnReplayMinigameLeft);

	FPDWInteractionPayload Payload{};
	Payload.Executioner = PC->GetPawn();
	Payload.InteractionInfo.AddTag(UPDWGameplayTagSettings::GetInteractionActionTag());
	InteractionComp->ProcessInteractionForced(Payload);
}

void APDWGameplayGameMode::OnReplayMinigameComplete(const FMiniGameEventSignature& inSignature)
{
	inSignature.MiniGameComponent->OnMinigameComplete.RemoveDynamic(this, &APDWGameplayGameMode::OnReplayMinigameComplete);
	for (TPair<TObjectPtr<UDataLayerAsset>, bool>& LayerInfo : PendingReplayMinigame.MinigameLayersInfo)
	{
		LayerInfo.Value = false;
	}
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), PendingReplayMinigame.MinigameLayersInfo);
	PendingReplayMinigame = {};
	ReturnToRyder();
}

void APDWGameplayGameMode::OnReplayMinigameLeft(const FMiniGameEventSignature& inSignature)
{
	inSignature.MiniGameComponent->OnMinigameLeft.RemoveDynamic(this, &APDWGameplayGameMode::OnReplayMinigameLeft);
	for (TPair<TObjectPtr<UDataLayerAsset>, bool>& LayerInfo : PendingReplayMinigame.MinigameLayersInfo)
	{
		LayerInfo.Value = false;
	}
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), PendingReplayMinigame.MinigameLayersInfo);
	PendingReplayMinigame = {};
	ReturnToRyder();
}

void APDWGameplayGameMode::OnBeforeSave()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if(!GameInstance)
		return;

	if (UFlowSubsystem* FlowSubsystem = GameInstance->GetSubsystem<UFlowSubsystem>())
	{
		UFlowSaveGame* NewSaveGame = Cast<UFlowSaveGame>(UGameplayStatics::CreateSaveGameObject(UFlowSaveGame::StaticClass()));
		FlowSubsystem->OnGameSaved(NewSaveGame);
		UPDWDataFunctionLibrary::SetFlowSaveGame(this, NewSaveGame);
	}
}

void APDWGameplayGameMode::ReturnToRyder()
{
	UWPHelperSubsystem* WPHelperSubsystem = UWPHelperSubsystem::Get(this);
	if (WPHelperSubsystem)
	{
		APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
		if (!PC)
			return;

		if (PC->GetIsOnVehicle())
		{
			PC->ChangeVehicle(false,false);
		}

		TArray<AActor*>ActorsToTeleport = { PC->GetPawn() };

		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			APDWPlayerController* PC2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
			if (PC2 && PC2->GetIsOnVehicle())
			{
				PC2->ChangeVehicle(false, false);
			}
			ActorsToTeleport.Add(PC2->GetPawn());
		}

		WPHelperSubsystem->Teleport(ActorsToTeleport, {BeforeReplayMinigameLocation});
		BeforeReplayMinigameLocation = FTransform();
	}
}

