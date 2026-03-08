// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWCutsceneFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "LevelSequenceActor.h"
#include "GameplayTagContainer.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "Data/PDWGameplayStructures.h"
#include "Engine/AssetManager.h"
#include "LevelSequencePlayer.h"
#include "Managers/PDWEventSubsytem.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "UI/Pages/PDWDialogueBasePage.h"
#include "Data/PDWGameSettings.h"
#include "QuestSettings.h"
#include "HAL/IConsoleManager.h"

#if WITH_EDITOR
#include "ToxicUtilitiesSetting.h"
#endif
#include "FlowSubsystem.h"
#include "NebulaVehicle/PDWVehiclePawn.h"

//const FName UPDWCutsceneFSMState::TO_NEXT_ID = FName("ToNext");

static TAutoConsoleVariable<int32> CVarPDWSkipAllCutscenes(
	TEXT("pdw.SkipAllCutscenes"),
	0,
	TEXT("If non-zero, skips all cutscenes at FSM entry."),
	ECVF_Default);

void UPDWCutsceneFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	const bool bSkipAllCutscenes =
#if WITH_EDITOR
		UToxicUtilitiesSetting::Get()->bAutoSkipAllCutscenes ||
#endif
		CVarPDWSkipAllCutscenes.GetValueOnGameThread() != 0;

	if (bSkipAllCutscenes)
	{
		Super::OnFSMStateEnter_Implementation(InOption);
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName(), "");
		return;
	}

	UPDWGameplayFunctionLibrary::ApplyQualitySettings(this, "Cutscene");

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnCutsceneStartEvent();
	}

	UGameplayStatics::SetForceDisableSplitscreen(this,true);

	UPDWGameplayFunctionLibrary::SetVehiclesPhysiscs(this, false);

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	
	if (GM && GM->FSMHelper)
	{
		CurrentSequenceCollection = GM->FSMHelper->PendingSequence;
	}

	if (CurrentSequenceCollection.SequenceCollectionNew.Num() > 0)
	{
		PageClass = CurrentSequenceCollection.SequenceCollectionNew[0].IsCinematic ? DialogueCinematic : DialogueCutscene;
	}

	Super::OnFSMStateEnter_Implementation(InOption);
	if (PageRef)
	{
		PageRef->SetVisibility(ESlateVisibility::Hidden);
	}
	if (CurrentSequenceCollection.SequenceCollectionNew.Num() > 0)
	{
		StartCutscene();
	}
	
	// #DEV <Check player controller target option> [#daniele.m, 29 September 2025, OnFSMStateEnter_Implementation]
	//when the cutscene start the player controller follow the camera so streaming source move. We need to spawna  streaming source on player location to maintain the location loaded.
	//probably should check player controller option on camera target to prevent that
	const FTransform PlayerTransform = UPDWGameplayFunctionLibrary::GetPlayerOne(this)->GetActorTransform();
	CinematicStreamingSource = GetWorld()->SpawnActor<AActor>(CinematicOriginStreamingSource,PlayerTransform);
}

void UPDWCutsceneFSMState::StartCutscene()
{
	SequenceActor = Cast<ALevelSequenceActor>(GetWorld()->SpawnActor<AActor>(ALevelSequenceActor::StaticClass(), FTransform()));	
	PlayNextSequence();
}

void UPDWCutsceneFSMState::PlayNextSequence()
{
	CurrentSequenceIndex++;
	if (CurrentSequenceCollection.SequenceCollectionNew.IsValidIndex(CurrentSequenceIndex))
	{
		CurrentSequenceData = CurrentSequenceCollection.SequenceCollectionNew[CurrentSequenceIndex];

		HandlePlayersVisibility(CurrentSequenceData.HidePlayer);
		HandleFramedActor();
		
		//Play Sequence
		LoadSoftObjsHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
			CurrentSequenceData.CutsceneSequence.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &UPDWCutsceneFSMState::OnSequenceLoaded),1000
		);
	}
	else
	{
		EndCutscene();
	}
}

void UPDWCutsceneFSMState::EndCutscene()
{
	if (CurrentSequenceCollection.bMakeTransitionAtEndCustcene)
	{
		UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
		if (UIManager)
		{
			UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::OnTransitionInEnd);
			UIManager->StartTransitionIn();
		}
	}
	else
	{
		OnTransitionInEnd();
	}
}

void UPDWCutsceneFSMState::OnTransitionInEnd()
{
	if (SequenceActor && SequenceActor->GetSequencePlayer())
	{
		SequenceActor->GetSequencePlayer()->Stop();
	}
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::OnTransitionInEnd);
	TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName(), "");
}

void UPDWCutsceneFSMState::HandlePlayersVisibility(bool IsHidden)
{
	APDWPlayerController* P1Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (!P1Controller || !P1Controller->GetPawn()) return;

	P1Controller->GetPawn()->SetActorHiddenInGame(IsHidden);
	if (P1Controller->GetIsOnVehicle() && P1Controller->GetVehicleInstance())
	{
		P1Controller->GetVehicleInstance()->SetActorHiddenInGame(IsHidden);
	}

	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P2Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		if (!P2Controller || !P2Controller->GetPawn()) return;

		P2Controller->GetPawn()->SetActorHiddenInGame(IsHidden);
		if (P2Controller->GetIsOnVehicle() && P2Controller->GetVehicleInstance())
		{
			P2Controller->GetVehicleInstance()->SetActorHiddenInGame(IsHidden);
		}
	}	
}

void UPDWCutsceneFSMState::SetUpCameraViewport()
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	AActor* ViewPortActor = PC->GetPawn();
	if (PC->GetIsOnVehicle())
	{
		ViewPortActor = PC->GetVehicleInstance();
	}
	PC->SetViewTarget(ViewPortActor, FViewTargetTransitionParams());

	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		PC = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
		ViewPortActor = PC->GetPawn();
		if (PC->GetIsOnVehicle())
		{
			ViewPortActor = PC->GetVehicleInstance();
		}
		PC->SetViewTarget(ViewPortActor, FViewTargetTransitionParams());
	}
}

void UPDWCutsceneFSMState::HandleFramedActor()
{
	UDefaultLevelSequenceInstanceData* SequenceData = Cast<UDefaultLevelSequenceInstanceData>(SequenceActor->DefaultInstanceData);
	FTransform FramedActorLocation = {};
	bool UseFramedActor = false;

	if (CurrentSequenceData.OnPlayer)
	{
		APawn* PlayerPawn = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->GetPawn();
		if (PlayerPawn)
		{
			FramedActorLocation = PlayerPawn->GetActorTransform();
			UseFramedActor = true;
		}
	}
	else
	{
		if (CurrentSequenceData.CustomLocation.GetLocation() != FVector::ZeroVector)
		{
			FramedActorLocation = CurrentSequenceData.CustomLocation;
			UseFramedActor = true;
		}
		else if (CurrentSequenceData.FramedActorKey != FGameplayTagContainer::EmptyContainer)
		{
			if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
			{
				FGameplayTagContainer TagContainer;

				for (const TWeakObjectPtr<UFlowComponent>& Component : FlowSubsystem->GetComponents<UFlowComponent>(CurrentSequenceData.FramedActorKey, EGameplayContainerMatchType::All, true))
				{
					if(!Component.Get())
						continue;

					AActor* ActorRef = Component.Get()->GetOwner();
					if(!ActorRef)
						continue;

					FramedActorLocation = ActorRef->GetActorTransform();
					UseFramedActor = true;
					break;
				}
			}

			if (!UseFramedActor)
			{
				//if is not loaded i search in the questconfig list
				UQuestSettings* QuestSettings = UQuestSettings::Get();
				UPDWQuestConfig* QuestConfig = Cast<UPDWQuestConfig>(QuestSettings->QuestConfig.TryLoad());
				for (FPDWQuestTargetLocs& TargetLoc : QuestConfig->TargetLocations)
				{
					if (TargetLoc.TagContainer.HasAllExact(CurrentSequenceData.FramedActorKey))
					{
						FramedActorLocation = TargetLoc.Locations.Num() > 0 ? FTransform(TargetLoc.Locations[0]) : FTransform();
						UseFramedActor = true;
						break;
					}
				}
			}
		}
	}

	if (UseFramedActor)
	{
		SequenceActor->bOverrideInstanceData = true;
		SequenceData->TransformOrigin = FTransform(FramedActorLocation);
	}
}

void UPDWCutsceneFSMState::OnSequenceLoaded()
{
	if (CurrentSequenceCollection.SequenceCollectionNew.IsValidIndex(CurrentSequenceIndex - 1))
	{
		if (CurrentSequenceData.IsCinematic != CurrentSequenceCollection.SequenceCollectionNew[CurrentSequenceIndex - 1].IsCinematic)
		{
			PageClass = CurrentSequenceData.IsCinematic ? DialogueCinematic : DialogueCutscene;
			if (PageRef)
			{
				CreatePage();
				//GEngine->ForceGarbageCollection(true);
			}
		}
	}
	
	if (PageRef && CurrentSequenceData.Conversation.DialogueLines.Num() > 0 && !CurrentSequenceData.IsCinematic)
	{
		if (!CurrentSequenceCollection.SequenceCollectionNew[CurrentSequenceIndex].WaitStartDialogueEvent)
		{
			PageRef->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}				
	}

	SequenceActor->GetSequencePlayer()->Stop();
	SequenceActor->SetSequence(CurrentSequenceData.CutsceneSequence.Get());
	if (CurrentSequenceData.IsCinematic)
	{
		SequenceActor->GetSequencePlayer()->OnFinished.AddDynamic(this, &UPDWCutsceneFSMState::OnCurrentSequenceFinished);
	}
	else
	{ 
		SequenceActor->PlaybackSettings.bPauseAtEnd = true;
		SequenceActor->GetSequencePlayer()->SetPlaybackSettings(SequenceActor->PlaybackSettings);
	}
	
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);	
	if (GM)
	{
		GM->CurrentLevelSequencePlayer = SequenceActor->GetSequencePlayer();
	}

	SequenceActor->GetSequencePlayer()->Play();
	if (!CurrentSequenceData.IsCinematic)
	{
		if (!CurrentSequenceData.WaitStartDialogueEvent)
		{
			UPDWDialogueSubSystem::TriggerConversation(this, CurrentSequenceData.Conversation);
		}
	}
}

void UPDWCutsceneFSMState::OnCurrentSequenceFinished()
{	
	SequenceActor->GetSequencePlayer()->OnFinished.RemoveDynamic(this, &UPDWCutsceneFSMState::OnCurrentSequenceFinished);
	PlayNextSequence();
}

void UPDWCutsceneFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == UFlowDeveloperSettings::GetSkipCutsceneTag().ToString())
	{
		if (SequenceActor && SequenceActor->GetSequencePlayer())
		{
			SequenceActor->GetSequencePlayer()->OnFinished.RemoveDynamic(this, &UPDWCutsceneFSMState::OnCurrentSequenceFinished);
			EndCutscene();
		}
	}
	if (Action == UPDWGameSettings::GetStartConversationAction())
	{
		if (PageRef)
		{
			if (PageRef->GetVisibility() == ESlateVisibility::Hidden)
			{
				PageRef->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			UPDWDialogueBasePage* DialoguePage = Cast<UPDWDialogueBasePage>(PageRef);
			if (DialoguePage)
			{
				DialoguePage->StartConversation();
			}
		}
	}
	
	if (Action == UPDWGameSettings::GetSkipAction() && !CurrentSequenceData.IsCinematic)
	{
		if (PageRef)
		{
			UPDWDialogueBasePage* DialoguePage = Cast<UPDWDialogueBasePage>(PageRef);
			if (DialoguePage)
			{
				DialoguePage->OnSkipPressed();
			}
		}
	}

	if (Action == UPDWGameSettings::GetEndConversationAction())
	{
		if (!CurrentSequenceData.IsCinematic)
		{
			PlayNextSequence();
		}
	}

	if (Action == UPDWGameSettings::GetUIActionDialogueFromSequence())
	{
		PageRef->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UPDWDialogueSubSystem::TriggerConversation(this, CurrentSequenceData.Conversation);
	}
}

void UPDWCutsceneFSMState::OnFSMStateExit_Implementation()
{
	HandlePlayersVisibility(false);

	UPDWGameplayFunctionLibrary::SetVehiclesPhysiscs(this, true);

	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	if (PC)
	{
		if (PC->GetVehicleInstance())
		{
			PC->GetVehicleInstance()->SetPhysActiveCarBP(true);
		}
	}

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnCutsceneEndEvent();
	}
	if (SequenceActor)
	{
		SequenceActor->GetSequencePlayer()->Stop();
	}
	SetUpCameraViewport();

	if (CinematicStreamingSource)
	{
		CinematicStreamingSource->Destroy();
		CinematicStreamingSource = nullptr;
	}

	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		UGameplayStatics::SetForceDisableSplitscreen(this,false);
	}

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);	
	if (GM)
	{
		GM->CurrentLevelSequencePlayer = nullptr;
	}

	UPDWGameplayFunctionLibrary::RestoreQualitySettings(this, "Cutscene");

	Super::OnFSMStateExit_Implementation();
}
