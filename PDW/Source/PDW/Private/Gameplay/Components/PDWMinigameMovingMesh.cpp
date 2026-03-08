// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWMinigameMovingMesh.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "Managers/PDWEventSubsytem.h"

UPDWMinigameMovingMesh::UPDWMinigameMovingMesh()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetVisibility(false,true);
	ComponentTags.Add(UPDWGameSettings::GetMiniGameMovingMeshTag());
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPDWMinigameMovingMesh::PossesComponent(APDWPlayerController* inController,UPDWMovingObjectBehaviour* MovingBehaviour /*= nullptr*/)
{
	ControllerOwner = inController;
	bPossessed = true;
	SetVisibility(true,true);
	SetComponentTickEnabled(true);
	for (UPDWInteractionBehaviour* Behaviour : Behaviours)
	{
		Behaviour->SetExecutioner(inController);
		Behaviour->InitBehaviour(this,FGameplayTag::EmptyTag);
	}
	if (MovingBehaviour)
	{
		CurrentMovingObjectBehaviours.AddUnique(MovingBehaviour);
	}
}

void UPDWMinigameMovingMesh::UnpossesComponent(bool bUnInitializingBehaviour/* = false*/,APDWPlayerController* inController/* = nullptr*/)
{
	ControllerOwner = nullptr;
	bPossessed = false;
	SetVisibility(false,true);
	SetComponentTickEnabled(false);
	if (!bUnInitializingBehaviour)
	{
		for (UPDWMovingObjectBehaviour* Behaviour : CurrentMovingObjectBehaviours)
		{
			if(Behaviour && inController)
			{
				Behaviour->RequestSceneComp(inController);
			}
		}
	}
	else
	{
		for (UPDWInteractionBehaviour* Behaviour : Behaviours)
		{
			if(Behaviour)
				Behaviour->StopBehaviour();
		}
	}
	CurrentMovingObjectBehaviours.Empty();
}

void UPDWMinigameMovingMesh::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	for (UPDWInteractionBehaviour* Behaviour : Behaviours)
	{
		Behaviour->TickBehaviour(DeltaTime);
	}
}

void UPDWMinigameMovingMesh::Reset()
{
	SetVisibility(false,true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWorldLocation(StartingPosition);
}

void UPDWMinigameMovingMesh::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

bool UPDWMinigameMovingMesh::GetPossessed() const
{
	return bPossessed;
}

void UPDWMinigameMovingMesh::BeginPlay()
{
	Super::BeginPlay();
	StartingPosition = GetComponentLocation();
}