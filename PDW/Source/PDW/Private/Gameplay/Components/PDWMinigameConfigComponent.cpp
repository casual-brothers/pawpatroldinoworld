// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameSettings.h"
#include "PDWGameInstance.h"
#include "Gameplay/Components/PDWSpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "ActorSequenceComponent.h"
#include "FlowComponent.h"
#include "FlowAsset.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/FLOW/PDWMinigameFlowAsset.h"
#include "Gameplay/Interfaces/PDWMinigameActorInterface.h"
#include "Gameplay/Components/PDWMinigameTargetComponent.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/Interaction/PDWStartMiniGameBehaviour.h"
#include "Gameplay/Actors/PDWSplineActor.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Managers/PDWUIManager.h"


UPDWMinigameConfigComponent::UPDWMinigameConfigComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	ComponentTags.Add(UPDWGameSettings::GetMiniGameComponentTag());
}

void UPDWMinigameConfigComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	
	for (UPDWMiniGameInputBehaviour* InputBehaviour : MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour[EMiniGamePlayerType::P1].InputBehaviour)
	{
		InputBehaviour->CustomTick(DeltaTime);
	}
	if (CurrentConfiguration == EMiniGamePlayerConfiguration::MultiPlayer)
	{
		
		for (UPDWMiniGameInputBehaviour* InputBehaviour : MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour[EMiniGamePlayerType::P2].InputBehaviour)
		{
			InputBehaviour->CustomTick(DeltaTime);
		}
	}
}

void UPDWMinigameConfigComponent::TriggerBehaviours()
{
	if(!MinigameConfigurationDataInstance->MiniGameConfig.Contains(CurrentConfiguration))
		return;

	if(!MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour.Contains(EMiniGamePlayerType::P1))
		return;

	TMap<EMiniGamePlayerConfiguration,FMiniGameBehaviourConfigStruct> Test = MinigameConfigurationDataInstance->MiniGameConfig;
	for (UPDWMiniGameInputBehaviour* InputBehaviour : MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour[EMiniGamePlayerType::P1].InputBehaviour)
	{
		InputBehaviour->InitializeBehaviour(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this),this);
	}
	if (CurrentConfiguration == EMiniGamePlayerConfiguration::MultiPlayer)
	{
		if(!MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour.Contains(EMiniGamePlayerType::P2))
			return;

		for (UPDWMiniGameInputBehaviour* InputBehaviour : MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour[EMiniGamePlayerType::P2].InputBehaviour)
		{
			InputBehaviour->InitializeBehaviour(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this), this);
		}
	}
	UPDWHUDSubsystem::Get(this)->RequestMinigameData(MinigameConfigurationDataInstance);
	SetComponentTickEnabled(true);
}

void UPDWMinigameConfigComponent::CleanBehaviours()
{
	if(!MinigameConfigurationDataInstance->MiniGameConfig.Contains(CurrentConfiguration))
		return;

	if(!MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour.Contains(EMiniGamePlayerType::P1))
		return;

	for (UPDWMiniGameInputBehaviour* InputBehaviour : MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour[EMiniGamePlayerType::P1].InputBehaviour)
	{
		InputBehaviour->UninitializeBehaviour();
	}
	if (CurrentConfiguration == EMiniGamePlayerConfiguration::MultiPlayer)
	{
		if(!MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour.Contains(EMiniGamePlayerType::P2))
			return;

		for (UPDWMiniGameInputBehaviour* InputBehaviour : MinigameConfigurationDataInstance->MiniGameConfig[CurrentConfiguration].MiniGameBehaviour[EMiniGamePlayerType::P2].InputBehaviour)
		{
			InputBehaviour->UninitializeBehaviour();
		}
	}
}

bool UPDWMinigameConfigComponent::SwapCharacter(APDWPlayerController* inController)
{
	bool bWaitForSwap = false;
	if (!SwapConfig.AllowedPup.IsValid())
	{
		return bWaitForSwap;
	}

	ensureMsgf(inController, TEXT("UPDWStartMiniGameBehaviour::SwapCharacter - Controller is not a PDWPlayerController"));
	
	const bool bIsOnVehicle = inController->GetIsOnVehicle();
	const bool bPupOnly = (SwapConfig.AutoSwapConfiguration & (uint8)EAutoSwapType::PupOnly) != 0;
	const bool bVehicleOnly = (SwapConfig.AutoSwapConfiguration & (uint8)EAutoSwapType::VehicleOnly) != 0;
	const bool bNeedSwapPup = (inController->GetPDWPlayerState()->GetCurrentPup() != SwapConfig.AllowedPup);

	const bool bForceVehicleSwap = bIsOnVehicle && bVehicleOnly && bNeedSwapPup;
	ForceVehicleSwapMap.Add(inController,bForceVehicleSwap);
	//I'M WRONG PUP
	if (bNeedSwapPup)
	{
		bWaitForSwap = true;
	
		//im wrong pup and is pup only or i'm already on a car and is vehicle only i need to bind on swap character.
		inController->OnPupCustomizationCompleted.AddUniqueDynamic(this,&UPDWMinigameConfigComponent::OnSwapPupComplete);
		inController->ChangeCharacter(SwapConfig.AllowedPup);

	}
	else
	{

		//It's vehicle only so i need to swap into vehicle or i'ts pup only but i'm on a vehicle.
		if ((bVehicleOnly && !bIsOnVehicle) || (bPupOnly && bIsOnVehicle))
		{
			bWaitForSwap = true;
			inController->OnSwapVehicleCompleted.AddUniqueDynamic(this, &UPDWMinigameConfigComponent::OnSwapVehicleComplete);
			inController->ChangeVehicle(false, false);
		}
	}
	
	return bWaitForSwap;
}

void UPDWMinigameConfigComponent::FinishControllerInitialization(APDWPlayerController* inController)
{
	if (bDisableVehiclePhysics)
	{
		inController->GetPDWPlayerState()->SetMiniGameVehiclePhysicsOff(true);
	}

	if (inController->GetIsOnVehicle())
	{
		if (APDWVehiclePawn* Vehicle = inController->GetVehicleInstance())
		{
			if (bDisableVehiclePhysics)
			{
				Vehicle->SetPhysActiveCarBP(false);
			}
			PlayOpeningMontage(Vehicle);
		}
	}

	inController->GetInteractionComponent()->AddAllowedInteraction(MinigameConfigurationDataInstance->MiniGameSpecificInteractions);
	inController->GetInteractionComponent()->AddOwnerInteractionStates(MinigameConfigurationDataInstance->MiniGameSpecificInteractions);
	SwapsToWait--;
	TeleportPlayer(inController);
	if (bHidePlayerTwo && StoredPlayerControllers.IsValidIndex(1))
	{
		if (inController == StoredPlayerControllers[1])
		{
			HidePlayer(inController, true);
		}
	}

	if (SwapsToWait <= 0 && !bFlowStarted)
	{
		SetUpCamera(inController);
		bFlowStarted = true;
		UPDWMinigameFlowAsset* FlowAsset = Cast<UPDWMinigameFlowAsset>(MiniGameFlow->RootFlow);
		if (FlowAsset)
		{
			FlowAsset->SetMinigameTag(MinigameConfigurationDataInstance->MiniGameIdentifierTag);
		}

		APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
		if (GameMode)
		{
			GameMode->CurrentMinigame = this;
		}

		MiniGameFlow->StartRootFlow();
		MiniGameFlow->NotifyOwnerGraph(MinigameConfigurationDataInstance->MiniGameIdentifierTag);
		OnMinigameInitializationComplete.Broadcast(StoredPlayerControllers);
	}
}

void UPDWMinigameConfigComponent::OnSwapPupComplete(APDWPlayerController* inController)
{
	inController->OnPupCustomizationCompleted.RemoveDynamic(this, &UPDWMinigameConfigComponent::OnSwapPupComplete);

	const bool bIsOnVehicle = inController->GetIsOnVehicle();
	const bool bPupOnly = (SwapConfig.AutoSwapConfiguration & (uint8)EAutoSwapType::PupOnly) != 0;
	const bool bVehicleOnly = (SwapConfig.AutoSwapConfiguration & (uint8)EAutoSwapType::VehicleOnly) != 0;
	const bool bNeedSwapPup = (inController->GetPDWPlayerState()->GetCurrentPup() != SwapConfig.AllowedPup);

	if ((bVehicleOnly && !bIsOnVehicle) || (bPupOnly && bIsOnVehicle))
	{
		inController->OnSwapVehicleCompleted.AddUniqueDynamic(this,&UPDWMinigameConfigComponent::OnSwapVehicleComplete);
		inController->ChangeVehicle(false,false);
	}
	else if (ForceVehicleSwapMap.Contains(inController) && ForceVehicleSwapMap[inController])
	{
		inController->OnSwapVehicleCompleted.AddUniqueDynamic(this,&UPDWMinigameConfigComponent::OnSwapVehicleComplete);
		//inController->ChangeVehicle(true,false); //on swap pup complete there is also the swap into vehicle
	}
	else
	{
		FinishControllerInitialization(inController);
	}
}

void UPDWMinigameConfigComponent::OnSwapVehicleComplete(const bool bInVehicle,APDWPlayerController* inController)
{
	inController->OnSwapVehicleCompleted.RemoveDynamic(this,&UPDWMinigameConfigComponent::OnSwapVehicleComplete);
	FinishControllerInitialization(inController);
}

void UPDWMinigameConfigComponent::OnRequestToJoinAnswered(FString inAnswer,APDWPlayerController* inController)
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this,DialogInstance);
	DialogInstance = nullptr;
	if (inAnswer == "Yes")
	{
		OnJoinAccepted();
	}
	else
	{		
		UPDWInteractionReceiverComponent* InteractionComponent = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
		InteractionComponent->ResetCollisions();
		UPDWEventSubsytem::Get(this)->OnJoinRequestRefusedEvent();
	}
}

void UPDWMinigameConfigComponent::OnJoinAccepted()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		if (UIManager->IsTransitionOnWithScreenBlack())
		{
			InizializeMinigameMultiPlayerAfterTransition();
		}
		else
		{
			UIManager->StartTransitionIn();
			UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::InizializeMinigameMultiPlayerAfterTransition);
		}
	}
}

void UPDWMinigameConfigComponent::InitializeMiniGame(APDWPlayerController* inInteracter,const FSwapConfiguration& inSwapConfig)
{
	SwapConfig = inSwapConfig;
	InteracterPlayer = inInteracter;
	
	UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance->GetIsMultiPlayerOn())
	{
		if (bSkipJoinRequestMessage)
		{
			OnJoinAccepted();
		}
		else
		{
			TArray<ANebulaFlowPlayerController*> SyncControllers = TArray<ANebulaFlowPlayerController*>();
			const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
			ensure(GameMode);

			SyncControllers.Add(GameMode->GetPlayerControllerOne());
			SyncControllers.Add(GameMode->GetPlayerControllerTwo());
			DialogInstance = UNebulaFlowUIFunctionLibrary::ShowDialog(this, "RequestToJoin", [&](FString InString) {
				OnRequestToJoinAnswered(InString, InteracterPlayer);
				}, nullptr, SyncControllers);
		}
	}
	else
	{
		UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
		if (UIManager)
		{
			if (UIManager->IsTransitionOnWithScreenBlack())
			{
				InizializeMinigameSinglePlayerAfterTransition();
			}
			else
			{
				UIManager->StartTransitionIn();
				UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::InizializeMinigameSinglePlayerAfterTransition);
			}
		}
	}

}

void UPDWMinigameConfigComponent::InizializeMinigameSinglePlayerAfterTransition()
{
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::InizializeMinigameSinglePlayerAfterTransition);
	//trigger initialization state nebula flow
	UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	/*
	-SET UP CAMERA
	-TELEPORT PUPS

	*/
	
	OnMinigameInitialization.Broadcast();
	StoredPlayerControllers.AddUnique(InteracterPlayer);
	const bool bWaitForSwap = SwapCharacter(InteracterPlayer);
	CurrentConfiguration = EMiniGamePlayerConfiguration::SinglePlayer;

	//#TODO : if needed turn them into behaviours
	//SetUpCamera(InteracterPlayer);
	/*TeleportPlayer(InteracterPlayer);*/
		
	FillCachedStates(0,InteracterPlayer);

	//Add specific tag container so only when minigame is playing you can interact with some objects

	//if (bDisableVehiclePhysics)
	//{
	//	InteracterPlayer->GetPDWPlayerState()->SetMiniGameVehiclePhysicsOff(true);
	//}

	InitializeMinigameTargetActors();

	if(!bWaitForSwap)
	{
		FinishControllerInitialization(InteracterPlayer);
	}
	else
	{
		SwapsToWait++;
	}
}

void UPDWMinigameConfigComponent::InizializeMinigameMultiPlayerAfterTransition()
{
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::InizializeMinigameMultiPlayerAfterTransition);
	UPDWEventSubsytem::Get(this)->OnMinigameInitializationEvent();
	OnMinigameInitialization.Broadcast();
	//HANDLE SWAP CHARACTER
	StoredPlayerControllers.AddUnique(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this));
	StoredPlayerControllers.AddUnique(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this));
	CurrentConfiguration = EMiniGamePlayerConfiguration::MultiPlayer;
	int32 Index = 0;
	SwapsToWait = 0;
	for (APDWPlayerController* Controller : StoredPlayerControllers)
	{
		const bool bWaitForSwap = SwapCharacter(Controller);
		//TeleportPlayer(Controller,Index);
		FillCachedStates(Index,Controller);

		if (bWaitForSwap)
		{
			SwapsToWait++;
		}
		else
		{
			FinishControllerInitialization(Controller);
		}
		++Index;
	}

	//if (SwapsToWait == 0)
	//{
	//	for (APDWPlayerController* Controller : StoredPlayerControllers)
	//	{
	//		FinishControllerInitialization(Controller);
	//	}
	//}
	UGameplayStatics::SetForceDisableSplitscreen(this,true);
	//SetUpCamera(StoredPlayerControllers[0]);
	InitializeMinigameTargetActors();
}

void UPDWMinigameConfigComponent::InitializeTargetActorsBehaviour()
{
	if (bInitializeTargetActorsOnMinigameStart)
	{
		for (AActor* Actor : MinigameTargetActors)
		{
			if (UPDWInteractionReceiverComponent* InteractionReceiver = Actor->FindComponentByClass<UPDWInteractionReceiverComponent>())
			{
				InteractionReceiver->InitInteractions();
			}
		}
	}
}

void UPDWMinigameConfigComponent::NotifyEvent(const FGameplayTag& inEvent)
{
	if (FunctionCheck.Contains(inEvent))
	{
		if(UFunction* Function = FunctionCheck[inEvent].ResolveMember<UFunction>(GetClass()))
		{
			FMinigameFlowEventSignature Event;
			Event.inEventTag = inEvent;
			Event.MiniGameComponent = this;

			FStructOnScope FuncParam(Function);

			this->ProcessEvent(Function,&Event);
		}
	}
}

void UPDWMinigameConfigComponent::OnNotifySuccessFromMinigameActor(const FInteractionEventSignature& inEvent)
{
	//#TODO: for now we just complete the minigame with one condition completed. If more condition can be present then adjust code.

	if (MinigameConfigurationDataInstance->MiniGameObjectiveConfig.Contains(inEvent.EventTag))
	{	
		//UPDWEventSubsytem::Get(this)->OnResetHintTimerRequestEvent();
		if (inEvent.MinigameActorTarget)
		{
			UPDWEventSubsytem::Get(this)->OnTargetDeactivationEvent(inEvent.MinigameActorTarget);
		}
		OnNotifySuccessFromMinigame.Broadcast(inEvent);
		FPDWInteractionPayload PayLoad;
		FPDWInteractReceiverPayload ReceiverPayload;
		ReceiverPayload.ReceiverActor = GetOwner();
		ReceiverPayload.ReceiverStates = FGameplayTag(); //VERIFY IF NEEDED
		if (MinigameConfigurationDataInstance->MiniGameObjectiveConfig[inEvent.EventTag]->EvaluateCondition(PayLoad,ReceiverPayload))
		{
			SetComponentTickEnabled(false);
			MiniGameFlow->NotifyOwnerGraph(UPDWGameplayTagSettings::GetMinigameTransitionToComplete());
		}
	}
}

UFlowComponent* UPDWMinigameConfigComponent::GetMinigameFlowComponent()
{
	return MiniGameFlow;
}

void UPDWMinigameConfigComponent::SetMinigameState(const FGameplayTag& inState)
{
	CurrentState = inState;
	GenerateMiniGameConfigurationInstance();
}

void UPDWMinigameConfigComponent::HidePlayer(APDWPlayerController* inController,bool bHide)
{
	if (inController)
	{

		if (inController->GetIsOnVehicle())
		{
			inController->GetVehicleInstance()->Hide(bHide);
			inController->GetPupInstance()->Hide(bHide,false);
		}
		else
		{
			inController->GetPupInstance()->Hide(bHide);
		}
	}
}

FGameplayTag UPDWMinigameConfigComponent::GetCurrentState() const
{
	return CurrentState;
}

void UPDWMinigameConfigComponent::GetPlayerControllers(TArray<APDWPlayerController*>& outControllers)
{
	outControllers = StoredPlayerControllers;
}

void UPDWMinigameConfigComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = MiniGameStartingState;
	MiniGameFlow = GetOwner()->FindComponentByClass<UFlowComponent>();
	ensureMsgf(MiniGameFlow, TEXT("UPDWMinigameConfigComponent::BeginPlay - MiniGameFlow not found on owner actor %s"), *GetOwner()->GetName());
	GenerateMiniGameConfigurationInstance();
	UPDWEventSubsytem::Get(this)->OnMinigameSpawned.Broadcast(this);
}

void UPDWMinigameConfigComponent::PlayClosingMontage(APDWVehiclePawn* Vehicle)
{
	if(!bUseVehicleMontage || !VehicleMontage)
		return;
	USkeletalMeshComponent* VehicleMesh = Vehicle->FindComponentByTag<USkeletalMeshComponent>("ComponentTag.VehicleMesh");
	if (VehicleMesh)
	{
		VehicleMesh->GetAnimInstance()->Montage_JumpToSection("End", VehicleMontage);
	}
}

void UPDWMinigameConfigComponent::PlayOpeningMontage(APDWVehiclePawn* Vehicle)
{
	if(!bUseVehicleMontage || !VehicleMontage)
		return;
	
	USkeletalMeshComponent* VehicleMesh = Vehicle->FindComponentByTag<USkeletalMeshComponent>("ComponentTag.VehicleMesh");
	if (VehicleMesh)
	{
		VehicleMesh->GetAnimInstance()->Montage_Play(VehicleMontage);
	}
}

void UPDWMinigameConfigComponent::GenerateMiniGameConfigurationInstance()
{
	ensureMsgf(StateMiniGameConfigurationData[CurrentState], TEXT("UPDWMinigameConfigComponent::BeginPlay - MinigameConfigurationDataInstance not set on component %s"), *GetName());
	if (!StateMiniGameConfigurationData[CurrentState])
	{
		return;
	}
	MinigameConfigurationDataInstance = NewObject<UPDWMinigameConfigData>(this, StateMiniGameConfigurationData[CurrentState]);
	MiniGameFlow->AddIdentityTag(MinigameConfigurationDataInstance->MiniGameIdentifierTag);
	UPDWEventSubsytem::Get(this)->BindToInteractionEvents(MinigameConfigurationDataInstance->MiniGameIdentifierTag)->AddUniqueDynamic(this, &UPDWMinigameConfigComponent::OnNotifySuccessFromMinigameActor);
}

void UPDWMinigameConfigComponent::FillCachedStates(const int32 inControllerIndex, APDWPlayerController* inController)
{
	FMinigameStatesCached CachedStateToFill;

	CachedStateToFill.PlayerStates = inController->GetInteractionComponent()->GetOwnerStates();
	CachedStateToFill.PlayerAllowedInteractions = inController->GetInteractionComponent()->GetAllowedInteractions();
	
	FGameplayTagContainer TagToRemove;
	for (const FGameplayTag& Tag : CachedStateToFill.PlayerStates)
	{
		if (Tag.RequestDirectParent().GetTagName() == "Input" || Tag.GetTagName() == "InteractionType.Minigame")
		{
			TagToRemove.AddTag(Tag);
		}
	}
	CachedStateToFill.PlayerStates.RemoveTags(TagToRemove);

	if (inControllerIndex == 0)
	{
		PlayerOneCachedState = CachedStateToFill;
	}
	else
	{
		PlayerTwoCachedState = CachedStateToFill;
	}
}

void UPDWMinigameConfigComponent::InitializeMinigameTargetActors()
{
	//chek if i need to pass also a playercontroller
	for (AActor* Actor : MinigameTargetActors)
	{
		if (Actor)
		{
			if (IPDWMinigameActorInterface* InterfaceActor = Cast<IPDWMinigameActorInterface>(Actor))
			{
				InterfaceActor->GetMinigameTargetComponent()->RegisterMinigameOwner(this);	
				InterfaceActor->GetMinigameTargetComponent()->OnMinigameStarted();
			}												
			else if (Actor->Implements<UPDWMinigameActorInterface>())
			{
				IPDWMinigameActorInterface::Execute_BP_GetMinigameTargetComponent(Actor)->RegisterMinigameOwner(this);
				IPDWMinigameActorInterface::Execute_BP_GetMinigameTargetComponent(Actor)->OnMinigameStarted();
			}
		}
	}
	InitializeTargetActorsBehaviour();
}

void UPDWMinigameConfigComponent::UnInitializeMinigameTargetActors(bool bReset)
{
	for ( int32 i = MinigameTargetActors.Num()-1; i>=0; i--)
	{
		if (MinigameTargetActors[i]->IsValidLowLevel())
		{
			if (IPDWMinigameActorInterface* InterfaceActor = Cast<IPDWMinigameActorInterface>(MinigameTargetActors[i]))
			{
				if (InterfaceActor->GetMinigameTargetComponent())
				{
					if (bReset)
					{
						InterfaceActor->GetMinigameTargetComponent()->ResetActor();
					}
					else
					{
						InterfaceActor->GetMinigameTargetComponent()->OnMinigameEnded();
					}
				}
			}												
			else if (MinigameTargetActors[i]->Implements<UPDWMinigameActorInterface>())
			{
				if (bReset)
				{
					IPDWMinigameActorInterface::Execute_BP_GetMinigameTargetComponent(MinigameTargetActors[i])->ResetActor();
				}
				else
				{
					IPDWMinigameActorInterface::Execute_BP_GetMinigameTargetComponent(MinigameTargetActors[i])->OnMinigameEnded();
				}
			}
		}
	}
}

void UPDWMinigameConfigComponent::SetUpCamera(APDWPlayerController* inController)
{
	if (!MinigameConfigurationDataInstance->CameraConfig.bUseMinigameCamera)
	{
		return;
	}

	ControllerToUseForCamera = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	//if (UPDWGameplayFunctionLibrary::IsSecondPlayer(inController))
	//{
	//	ControllerToUseForCamera = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
	//}
	ControllerToUseForCamera->UsingMinigameCamera = true;
	ControllerToUseForCamera->SetViewTargetWithBlend(GetOwner(),MinigameConfigurationDataInstance->CameraConfig.BlendTime,MinigameConfigurationDataInstance->CameraConfig.BlendFunction);
	if (MinigameConfigurationDataInstance->CameraConfig.CameraTransitionType == EMiniGameCameraTransitionType::SequenceOverview)
	{
		UActorSequenceComponent* SequenceComp = GetOwner()->FindComponentByClass<UActorSequenceComponent>();
		SequenceComp->PlaySequence();
	}
}

void UPDWMinigameConfigComponent::BlendCameraBack()
{
	if (!MinigameConfigurationDataInstance->CameraConfig.bUseMinigameCamera || MinigameConfigurationDataInstance->CameraConfig.bSkipEndingMinicameraBlend)
	{
		return;
	}
	APawn* PawnToUse = ControllerToUseForCamera->GetPawn();
	if (ControllerToUseForCamera->GetIsOnVehicle())
	{
		PawnToUse = ControllerToUseForCamera->GetVehicleInstance();
	}
	ControllerToUseForCamera->SetViewTargetWithBlend(PawnToUse,MinigameConfigurationDataInstance->CameraConfig.BlendTime,MinigameConfigurationDataInstance->CameraConfig.BlendFunction);
}

void UPDWMinigameConfigComponent::TeleportPlayer(APDWPlayerController* inController, int32 Index /*= 0*/)
{

	FString ComponentTag = UPDWGameSettings::GetMiniGameStartintPositionTag().ToString();
	if (inController == UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this))
	{
		Index = 1;
	}
	FString FinalTag = ComponentTag;
	FinalTag.Append(FString::FromInt(Index));
	const UArrowComponent* PlayerPosition = GetOwner()->FindComponentByTag<UArrowComponent>(*FinalTag);
	if (PlayerPosition)
	{
		if (inController->GetIsOnVehicle())
		{
			inController->GetVehicleInstance()->SetActorLocationAndRotation(PlayerPosition->GetComponentLocation(), PlayerPosition->GetComponentRotation());
		}
		else
		{
			inController->GetPawn()->SetActorLocationAndRotation(PlayerPosition->GetComponentLocation(), PlayerPosition->GetComponentRotation());
		}
	}

	
}

void UPDWMinigameConfigComponent::MiniGameComplete()
{
	FMiniGameEventSignature Signature;
	Signature.EventTag = MinigameConfigurationDataInstance->MiniGameIdentifierTag;
	Signature.MiniGameComponent = this;
	bFlowStarted=false;
	MiniGameLeft(false);
	OnMinigameComplete.Broadcast(Signature);
}

void UPDWMinigameConfigComponent::MiniGameLeft(bool bReset/* = true*/)
{	
	ForceVehicleSwapMap.Empty();
	CleanBehaviours();
	int32 Index = 0;
	for (APDWPlayerController* Controller : StoredPlayerControllers)
	{
		if (Controller)
		{
			Controller->UsingMinigameCamera = false;
			Controller->GetInteractionComponent()->SetAllowedInteraction(PlayerOneCachedState.PlayerAllowedInteractions);
			Controller->GetInteractionComponent()->SetOwnerInteractionStates(PlayerOneCachedState.PlayerStates);
			if (bDisableVehiclePhysics)
			{
				Controller->GetPDWPlayerState()->SetMiniGameVehiclePhysicsOff(false);
			}

			if (Controller->GetIsOnVehicle())
			{
				if (APDWVehiclePawn* Vehicle = Controller->GetVehicleInstance())
				{
					if (bDisableVehiclePhysics)
					{
						Vehicle->SetPhysActiveCarBP(true);
					}
					PlayClosingMontage(Vehicle);
				}
			}
			if (bReset)
			{
				TeleportPlayer(Controller,Index);
			}
			else
			{
				// #DEV <specific logic for water run, not the best place to put it, but no time> [#daniele.m, 30 October 2025, MiniGameLeft]
				SplineRunCompleteTeleport(Index, Controller);
			}
			if (Index != 0 && bHidePlayerTwo)
			{
				HidePlayer(Controller,false);
			}
			Index++;
		}
	}
	//if this became too hard to maintain, just clear the instance and recreate a new one.
	for (auto& [Tag, Condition] : MinigameConfigurationDataInstance->MiniGameObjectiveConfig)
	{
		Condition->ResetCondition();
	}
	BlendCameraBack();
	UGameplayStatics::SetForceDisableSplitscreen(this,false);
	StoredPlayerControllers.Empty();
	UnInitializeMinigameTargetActors(bReset);
	if (bReset)
	{
		FMiniGameEventSignature Signature;
		Signature.EventTag = MinigameConfigurationDataInstance->MiniGameIdentifierTag;
		Signature.MiniGameComponent = this;
		OnMinigameLeft.Broadcast(Signature);
		SwapsToWait = 0;
		bFlowStarted = false;
	}
	UPDWEventSubsytem::Get(this)->OnMinigameCompleteOrLeftEvent(GetOwner());
	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->CurrentMinigame = nullptr;
	}
}

void UPDWMinigameConfigComponent::SplineRunCompleteTeleport(const int32 Index, APDWPlayerController* Controller)
{
	if (bUseSpecificEndPosition)
	{
		if (GetOwner())
		{
			if (APDWSplineActor* SplineActor = Cast<APDWSplineActor>(GetOwner()))
			{
				TArray<AActor*> EndingPoints = SplineActor->GetPlayersEndPoint();
				if (EndingPoints.Num())
				{
					if (EndingPoints.IsValidIndex(Index) && EndingPoints[Index])
					{
						if (Controller->GetIsOnVehicle())
						{
							Controller->GetVehicleInstance()->SetActorLocationAndRotation(EndingPoints[Index]->GetActorLocation(), EndingPoints[Index]->GetActorRotation());
						}
						else
						{
							Controller->GetPawn()->SetActorLocationAndRotation(EndingPoints[Index]->GetActorLocation(), EndingPoints[Index]->GetActorRotation());
						}
					}
				}
			}
		}
	}
	}

void UPDWMinigameConfigComponent::RestartMiniGame()
{
	BlendCameraBack();
}

void UPDWMinigameConfigComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//VERIFY IF I SHOULD ALSO REMOVE THE ENTRY IN THE MAP
	//UPDWEventSubsytem::Get(this)->BindToInteractionEvents(MiniGameIdentifierTag)->RemoveDynamic(this, &UPDWMinigameConfigComponent::OnNotifySuccessFromMinigameActor);
	Super::EndPlay(EndPlayReason);
}

