// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWMovingNPCActorsSpawner.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Animation/PDWDinoAnimInstance.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Pawns/PDWCharacter.h"

void APDWMovingNPCActorsSpawner::BeginPlay()
{
	Super::BeginPlay();
	RandomAnimationInterval = FMath::RandRange(MinRandomAnimationInterval,MaxRandomAnimationInterval);

	if (RuntimeSpawnedActorList.Num() > 0)
	{
		ActorOnSpline = RuntimeSpawnedActorList[0];
		if(!ActorOnSpline) return;

		USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(ActorOnSpline->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if(!SkeletalMesh) return;

		AnimInstanceRef = Cast<UPDWDinoAnimInstance>(SkeletalMesh->GetAnimInstance());
		if(!AnimInstanceRef) return;
		AnimInstanceRef->SetSpeedOverride(GetActorsSpeed());

		ReceiverCompRef = Cast<UPDWInteractionReceiverComponent>(ActorOnSpline->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
		if(!ReceiverCompRef) return;
		ReceiverCompRef->OnInteract.AddUniqueDynamic(this, &APDWMovingNPCActorsSpawner::OnInteractionStart);
		ReceiverCompRef->OnBehaviourFinished.AddUniqueDynamic(this, &APDWMovingNPCActorsSpawner::OnInteractionEnd);
	}
}

void APDWMovingNPCActorsSpawner::OnInteractionStart(const FPDWInteractionPayload& Payload)
{
	if (!Payload.InteractionType.MatchesTag(UPDWGameplayTagSettings::GetInteractionActionTag())) //Ignore overlap interaction
	{
		DoingInteraction = true;
	}
}

void APDWMovingNPCActorsSpawner::OnInteractionEnd(UPDWInteractionBehaviour* Behaviour)
{
	DoingInteraction = false;
}

void APDWMovingNPCActorsSpawner::Tick(float DeltaTime)
{
	if (GetActorsSpeed() > 0.f)
	{
		CheckPlayerDistance();

		if (AnimInstanceRef)
		{
			AnimInstanceRef->SetSpeedOverride(IsPlayerClose || IsPlayerTooFar ? 0.f : GetActorsSpeed());
		}
	}


	if (!DoingAnimation && !IsPlayerClose && !IsPlayerTooFar && !DoingInteraction)
	{
		HandleRandomAnimation(DeltaTime);
		if (GetActorsSpeed() > 0.f)
		{
			Super::Tick(DeltaTime);	
		}
	}
}

void APDWMovingNPCActorsSpawner::HandleRandomAnimation(float DeltaTime)
{
	RandomAnimationTimer += DeltaTime;
	if (RandomAnimationTimer >= RandomAnimationInterval)
	{
		RandomAnimationTimer = 0.f;
		if (RandomAnimations.Num() == 0 || !AnimInstanceRef) return;
		
		DoingAnimation = AnimInstanceRef->PlayMontageByTag(RandomAnimations[FMath::RandRange(0, RandomAnimations.Num() - 1)]);
		if (DoingAnimation)
		{
			AnimInstanceRef->OnMontageEnded.AddUniqueDynamic(this, &APDWMovingNPCActorsSpawner::OnMontageEnd);
		}
	}
}

void APDWMovingNPCActorsSpawner::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	AnimInstanceRef->OnMontageEnded.RemoveDynamic(this, &APDWMovingNPCActorsSpawner::OnMontageEnd);
	DoingAnimation = false;
	RandomAnimationInterval = FMath::RandRange(MinRandomAnimationInterval,MaxRandomAnimationInterval);
}

void APDWMovingNPCActorsSpawner::Spawn()
{
	if (MovingActorsSpawnerSetupList.Num() > 1 || (MovingActorsSpawnerSetupList.Num() == 1 && MovingActorsSpawnerSetupList[0].ActorsToSpawn.Default > 1))
	{
		MovingActorsSpawnerSetupList[0].ActorsToSpawn = 1;
		FText Message = NSLOCTEXT("APDWMovingNPCActorsSpawner", "Error", "Only one dino per spline!");
		FMessageDialog::Open(EAppMsgType::Ok, Message);
	}

	//if (HasAnyFlags(RF_ClassDefaultObject) || IsTemplate()) return;

	Super::Spawn();
}

void APDWMovingNPCActorsSpawner::CheckPlayerDistance()
{
	if(!ActorOnSpline) return;
	FVector SpawnedActorLocation = ActorOnSpline->GetActorLocation();

	APDWPlayerController* P1Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (!P1Controller) return;
	APawn* P1Pawn = nullptr;
	if (P1Controller->GetIsOnVehicle())
	{
		P1Pawn = P1Controller->GetVehicleInstance();
	}
	else
	{
		P1Pawn = P1Controller->GetPupInstance();
	}
	if(!P1Pawn) return;
	
	float P1DistanceToCheck = FVector::Dist(P1Pawn->GetActorLocation(), SpawnedActorLocation);
	float P1PlayerDistance = P1Controller->GetIsOnVehicle() ? StopDistanceFromVehicles : StopDistanceFromPlayers;

	IsPlayerClose = P1DistanceToCheck < P1PlayerDistance;
	IsPlayerTooFar = P1DistanceToCheck > OptimizationDistance;

	if (!IsPlayerClose || !IsPlayerTooFar)
	{
		APDWPlayerController* P2Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		if (!P2Controller) return;
		APawn* P2Pawn = nullptr;
		if (P2Controller->GetIsOnVehicle())
		{
			P2Pawn = P2Controller->GetVehicleInstance();
		}
		else
		{
			P2Pawn = P2Controller->GetPupInstance();
		}
		if (!P2Pawn) return;

		float P2DistanceToCheck = FVector::Dist(P2Pawn->GetActorLocation(), SpawnedActorLocation);
		if (!IsPlayerClose)
		{
			float P2PlayerDistance = P2Controller->GetIsOnVehicle() ? StopDistanceFromVehicles : StopDistanceFromPlayers;
			IsPlayerClose = P2DistanceToCheck < P2PlayerDistance;
		}
		if (!IsPlayerTooFar)
		{
			IsPlayerTooFar = P2DistanceToCheck > OptimizationDistance;
		}
	}
}