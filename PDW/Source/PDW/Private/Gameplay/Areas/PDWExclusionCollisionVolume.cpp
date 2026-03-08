// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Areas/PDWExclusionCollisionVolume.h"
#include "Data/PDWGameSettings.h"
#include "Components/BrushComponent.h"

#define COLLISION_AUTOSWAP		ECC_GameTraceChannel8

APDWExclusionCollisionVolume::APDWExclusionCollisionVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetBrushComponent()->SetCollisionProfileName(UPDWGameSettings::GetExclusionVolumeProfileName().Name);
	bGenerateOverlapEventsDuringLevelStreaming = true;
}

void APDWExclusionCollisionVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if(!PlayerPawn) return;

	APDWPlayerController* PlayerController = Cast<APDWPlayerController>(PlayerPawn->GetController());
	if(!PlayerController) return;

	if (PlayerController->bDoingSwappingAction)
	{
		PlayerController->PendingExclusionVolume = this;
		PlayerController->CurrentSwapArea = nullptr;
		return;
	}

	UPDWInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UPDWInteractionComponent>();
	if (InteractionComp)
	{
		InteractionComp->SetCollisionResponseToChannel(COLLISION_AUTOSWAP, ECollisionResponse::ECR_Ignore);
	}
}

void APDWExclusionCollisionVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);



	UPDWInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UPDWInteractionComponent>();
	if (InteractionComp)
	{
		InteractionComp->SetCollisionResponseToChannel(COLLISION_AUTOSWAP, ECollisionResponse::ECR_Overlap);
	}
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if(!PlayerPawn) return;

	APDWPlayerController* PlayerController = Cast<APDWPlayerController>(PlayerPawn->GetController());
	if(!PlayerController) return;
	PlayerController->PendingExclusionVolume = nullptr;
}


