// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWTeleportActor.h"
#include "Managers/PDWUIManager.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWInputSubsystem.h"
#include "Managers/PDWEventSubsytem.h"

void APDWTeleportActor::StartTeleport(TArray<AActor*>& inTeleportingActors, const TArray<FTransform>& inPositions)
{
	TeleportingActors = inTeleportingActors;
	PositionsToTeleport = inPositions;
	//UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	//UIManager->StartTransitionIn();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			UPDWInputSubsystem::Get(this)->RemoveInputs(PC);
		}
	}
	//UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::OnTransitionInEnd);
	Super::StartTeleport(TeleportingActors, PositionsToTeleport);
}

void APDWTeleportActor::Teleport()
{
	//UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	//UIManager->StartTransitionOut();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			UPDWInputSubsystem::Get(this)->RestoreLastConfiguration(PC);
		}
	}
	Super::Teleport();
}

void APDWTeleportActor::OnTransitionInEnd()
{
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::OnTransitionInEnd);
	Super::StartTeleport(TeleportingActors, PositionsToTeleport);
}