// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Gameplay/Areas/PDWEnvironmentMultiplayerTravelArea.h"

#include "Actors/NebulaGraphicsSkyActor.h"
#include "Components/BrushComponent.h"
#include "Data/PDWGameSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "WPHelperSubsystem.h"

#include "Engine/TargetPoint.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
	
APDWEnvironmentMultiplayerTravelArea::APDWEnvironmentMultiplayerTravelArea()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetBrushComponent()->SetCollisionProfileName(UPDWGameSettings::GetAudioEnvironmentAreaProfileName().Name);
	bGenerateOverlapEventsDuringLevelStreaming = true;
}


void APDWEnvironmentMultiplayerTravelArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	ensure(GameMode);
	
	if (!UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		UPDWDataFunctionLibrary::SetPlayerAreaId(this, EnvironmentType);
		return;
	}
	APDWPlayerController* P1 = GameMode->GetPlayerControllerOne();
	APDWPlayerController* P2 = GameMode->GetPlayerControllerTwo();
	if (!P1 || !P2)
	{
		return;
	}
	FGameplayTag CurrentPlayerAreaId = UPDWDataFunctionLibrary::GetPlayerAreaId(this);
	if (CurrentPlayerAreaId == EnvironmentType) return;
	if (InsideTeleportPoints.Num() == 0) return;

	TArray<ANebulaFlowPlayerController*> SyncControllers = TArray<ANebulaFlowPlayerController*>();
	SyncControllers.Add(P1);
	SyncControllers.Add(P2);
	// This slot already has something
	ChangeAreaDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, ChangeAreaDialogID,
		[=, this](FString InResponse)
		{
			APawn* OverlapperPawn = Cast<APawn>(OtherActor);

			if (InResponse == "Yes")
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(ChangeAreaDialog, ChangeAreaDialog);
				if (InsideTeleportPoints.Contains(CurrentPlayerAreaId))
				{
					AActor* OtherPawn;
					if (OverlapperPawn && OverlapperPawn->GetController() == P1)
					{
						OtherPawn = P2->GetIsOnVehicle() ? P2->GetVehicleInstance() : P2->GetPawn();
					}
					else
					{
						OtherPawn = P1->GetIsOnVehicle() ? P1->GetVehicleInstance() : P1->GetPawn();
					}
					TArray<AActor*> ActorsArray = TArray<AActor*>{ OtherPawn };
					UWPHelperSubsystem::Get(this)->Teleport(ActorsArray, {InsideTeleportPoints[CurrentPlayerAreaId]->GetTransform()});
					
					UPDWDataFunctionLibrary::SetPlayerAreaId(this, EnvironmentType);
				}
			}
			else if (InResponse == "No")
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(ChangeAreaDialog, ChangeAreaDialog);
				if (OutsideTeleportPoints.Contains(CurrentPlayerAreaId))
				{
					TArray<AActor*> ActorsArray = TArray<AActor*>{ OverlapperPawn };
					UWPHelperSubsystem::Get(this)->Teleport(ActorsArray, {OutsideTeleportPoints[CurrentPlayerAreaId]->GetTransform()});
				}
			}
		}
	, P1, SyncControllers);
}

void APDWEnvironmentMultiplayerTravelArea::BeginPlay()
{
	Super::BeginPlay();
}