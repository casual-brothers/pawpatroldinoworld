// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDW_NavigationWaypoint.h"
#include "Managers/QuestSubsystem.h"

// Sets default values
APDW_NavigationWaypoint::APDW_NavigationWaypoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APDW_NavigationWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
	UQuestSubsystem* QuestSystem = UQuestSubsystem::Get(this);
	if (QuestSystem)
	{
		QuestSystem->AddWaypoint(this);
	}
}
