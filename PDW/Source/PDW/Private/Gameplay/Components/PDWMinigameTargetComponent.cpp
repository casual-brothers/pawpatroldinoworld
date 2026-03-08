// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWMinigameTargetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Gameplay/MiniGames/PDWMinigameGameplayBehaviour.h"
#include "Data/PDWGameSettings.h"

DECLARE_CYCLE_STAT(TEXT("UPDWMinigameTargetComponent ~ CycleActors"), STAT_CycleActors, STATGROUP_MinigameTargetComp);


UPDWMinigameTargetComponent::UPDWMinigameTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPDWMinigameTargetComponent::RegisterMinigameOwner(UPDWMinigameConfigComponent* inOwner)
{
	MinigameOwner = inOwner;
}

void UPDWMinigameTargetComponent::ResetActor()
{
	//TO IMPLEMENT
	//CHECK FROM SAVES THE INITIAL POSITION AND APPLY IT
}

void UPDWMinigameTargetComponent::SaveStartingState()
{
	//TO IMPLEMENT
	//SAVE INITIAL STATE OF THE ACTOR 
	//POSITION,STATE
}

void UPDWMinigameTargetComponent::OnMinigameStarted()
{
	BP_OnMinigameStarted();
}

void UPDWMinigameTargetComponent::OnMinigameEnded()
{
	BP_OnMinigameEnded();
	//if (bShouldBeDestroyedOnEnd)
	//{
	//	if (GetOwner()->IsValidLowLevel())
	//	{
	//		GetOwner()->Destroy();
	//	}
	//}
}

void UPDWMinigameTargetComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPDWMinigameTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}



