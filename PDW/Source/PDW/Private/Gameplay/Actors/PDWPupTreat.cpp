// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWPupTreat.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWPlayerState.h"
#include "Managers/PDWEventSubsytem.h"

void APDWPupTreat::OnPickUpStart(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	Super::OnPickUpStart(PickUp, PlayerController);
}

void APDWPupTreat::OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	//player state needed?
	if (!AlreadyTaken && PlayerController && PlayerController->PlayerState)
	{
		UPDWDataFunctionLibrary::AddCollectedTreat(this, CollectibleIndex);
		Cast<APDWPlayerState>(PlayerController->PlayerState)->ChangePupTreats(PointsGained);
	}

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnPupTreatCollectedEvent();
	}

	Super::OnPickUpEnd(PickUp, PlayerController);
}

void APDWPupTreat::BeginPlay()
{
	Super::BeginPlay();

	TArray<int32> CollectedTreats = UPDWDataFunctionLibrary::GetCollectedTreats(this);
	if (CollectedTreats.Contains(CollectibleIndex))
	{
		AlreadyTaken = true;
		CollectibleMeshComponent->SetMaterial(0, PickedUpMaterial);
	}	
}

void APDWPupTreat::PostEditImport()
{
	Super::PostEditImport();
	CollectibleIndex = -1;
}
