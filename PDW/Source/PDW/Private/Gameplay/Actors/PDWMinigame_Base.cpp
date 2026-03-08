// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWMinigame_Base.h"
#include "FlowComponent.h"
#include "Gameplay/Components/PDWMinigameMovingMesh.h"
#include "Data/PDWGameSettings.h"
#include "Math/RandomStream.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Components/PDWJigsawComponent.h"

// Sets default values
APDWMinigame_Base::APDWMinigame_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FlowComponent = CreateDefaultSubobject<UFlowComponent>(TEXT("FlowComponent"));
	MinigameRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MinigameRoot"));
	SetRootComponent(MinigameRoot);
	InteractionReceiver = CreateDefaultSubobject<UPDWInteractionReceiverComponent>(TEXT("InteractionReceiver"));
	InteractionReceiver->SetupAttachment(RootComponent);
	CameraMinigame = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraMinigame"));
	CameraMinigame->SetupAttachment(MinigameRoot);
}

//void APDWMinigame_Base::OnInteractionTriggered(const FPDWInteractionPayload& inPayLoad)
//{
//	TriggerMiniGame();
//}

// Called when the game starts or when spawned
void APDWMinigame_Base::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActorComponent*> Components = GetComponentsByTag(UPDWMinigameMovingMesh::StaticClass(),UPDWGameSettings::GetMiniGameMovingMeshTag());
	for (UActorComponent* Comp : Components)
	{
		if (Comp)
		{
			if (UPDWMinigameMovingMesh* MovingMeshComp = Cast<UPDWMinigameMovingMesh>(Comp))
			{
				MovingMeshList.AddUnique(MovingMeshComp);
			}
		}
	}
	if (MovingMeshList.Num())
	{
		FRandomStream RandomStream(FDateTime::Now().GetMillisecond());
		const int32 LastIndex = MovingMeshList.Num() - 1;
		for (int32 i = LastIndex; i > 0; --i)
		{
			const int32 Index = RandomStream.RandRange(0, i);
			if (i != Index)
			{
				MovingMeshList.Swap(i, Index);
			}
		}
		CachedMovingMeshList = MovingMeshList;
	}
	
	MultiplayerMovingMeshInstance = FindComponentByTag<UPDWMinigameMovingMesh>(MultiPlayerMovingMeshTag);

	GetComponents<UPDWJigsawComponent>(PuzzleSlots);

	MiniGameComponent = FindComponentByClass<UPDWMinigameConfigComponent>();
	MiniGameComponent->OnMinigameLeft.AddUniqueDynamic(this,&APDWMinigame_Base::OnMinigameLeft);
	if (bUseHintTimer)
	{
		MiniGameComponent->OnMinigameInitializationComplete.AddUniqueDynamic(this,&APDWMinigame_Base::OnMinigameInitializationComplete);
		UPDWEventSubsytem::Get(this)->OnResetHintTimerRequest.AddUniqueDynamic(this, &APDWMinigame_Base::ResetHintTimer);
	}	
}

void APDWMinigame_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(HintTimerHandle);
	MiniGameComponent->OnMinigameLeft.RemoveDynamic(this,&APDWMinigame_Base::OnMinigameLeft);
	MiniGameComponent->OnMinigameInitializationComplete.RemoveDynamic(this,&APDWMinigame_Base::OnMinigameInitializationComplete);
	UPDWEventSubsytem::Get(this)->OnResetHintTimerRequest.RemoveDynamic(this, &APDWMinigame_Base::ResetHintTimer);
	Super::EndPlay(EndPlayReason);
}

void APDWMinigame_Base::OnMinigameLeft(const FMiniGameEventSignature& inSignature)
{
	GetWorld()->GetTimerManager().ClearTimer(HintTimerHandle);
	for(UPDWMinigameMovingMesh* MovingMesh : CachedMovingMeshList)
	{
		MovingMesh->Reset();
	}
	for (UPDWJigsawComponent* JigSaw : PuzzleSlots)
	{
		JigSaw->Reset();
	}
	MovingMeshList = CachedMovingMeshList;

	if (InteractionReceiver)
	{
		InteractionReceiver->ResetCollisions();
	}
}

void APDWMinigame_Base::OnMinigameInitializationComplete(const TArray<APDWPlayerController*>& Controllers)
{
	MiniGameComponent->OnMinigameInitializationComplete.RemoveDynamic(this,&APDWMinigame_Base::OnMinigameInitializationComplete);
	GetWorld()->GetTimerManager().SetTimer(HintTimerHandle, this, &APDWMinigame_Base::TriggerHint, HintTimer, true);
	MiniGameComponent->GetMinigameTargetActors(Actors);
}

void APDWMinigame_Base::TriggerHint()
{
	BP_OnHintTriggered();
	const FGameplayTag MinigameID = GetMinigameConfigComp()->GetMinigameConfigurationData()->MiniGameIdentifierTag;
	UPDWEventSubsytem::Get(this)->OnHintRequestEvent(MinigameID);
	for (auto& Actor : Actors)
	{
		if (Actor && (!Actor->Tags.Contains("HintException")))
		{
			UPDWEventSubsytem::Get(this)->OnShowHintEvent(Actor->GetRootComponent(),MinigameID);
		}
	}
}

void APDWMinigame_Base::ResetHintTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(HintTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(HintTimerHandle, this, &APDWMinigame_Base::TriggerHint, HintTimer, true);
	for (auto& Actor : Actors)
	{
		if (Actor)
		{
			UPDWEventSubsytem::Get(this)->OnTargetDeactivationEvent(Actor);
		}
	}
}

// Called every frame
void APDWMinigame_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UPDWMinigameMovingMesh* APDWMinigame_Base::RequestMovingMeshComponent(APDWPlayerController* RequestingController)
{
	if (MovingMeshList.Num())
	{
		LastMovingMeshGiven = MovingMeshList.Pop();
		if(LastMovingMeshGiven)
			GivenMeshes.AddUnique(LastMovingMeshGiven);

		if (!LastControlledPiece.Contains(RequestingController))
		{
			LastControlledPiece.Add(RequestingController, LastMovingMeshGiven);
		}
		else
		{
			LastControlledPiece[RequestingController] = LastMovingMeshGiven;
		}
	}
	else
	{
		if (!UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			return nullptr;
		}
		else
		{
			for (auto& [Controller, Piece] : LastControlledPiece)
			{
				if (Controller != RequestingController && MultiplayerMovingMeshInstance->GetID() == NAME_None)
				{
					MultiplayerMovingMeshInstance->SetStaticMesh(Piece->GetStaticMesh());
					MultiplayerMovingMeshInstance->SetID(Piece->GetID());
					return MultiplayerMovingMeshInstance;
				}
			}
			return nullptr;
		}
	}
	return LastMovingMeshGiven;
}

void APDWMinigame_Base::GetMovingMeshes(TArray<UPDWMinigameMovingMesh*>& OutMovingMesh)
{
	OutMovingMesh = MovingMeshList;
}

UPDWMinigameConfigComponent* APDWMinigame_Base::GetMinigameConfigComp()
{
	return MiniGameComponent;
}

//void APDWMinigame_Base::CompleteMinigame()
//{
//	MinigameFinished.Broadcast(this);
//}
//
//void APDWMinigame_Base::TriggerMiniGame()
//{
//	FlowComponent->StartRootFlow();
//}

