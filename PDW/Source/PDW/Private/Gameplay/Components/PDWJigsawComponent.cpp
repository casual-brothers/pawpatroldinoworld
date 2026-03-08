// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWJigsawComponent.h"
#include "Gameplay/Components/PDWPuzzlePieceComponent.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Actors/PDWMinigame_Base.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

// Sets default values for this component's properties
UPDWJigsawComponent::UPDWJigsawComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


void UPDWJigsawComponent::Reset()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts
void UPDWJigsawComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPDWJigsawComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPDWJigsawComponent::OnEndOverlap);
	UPDWEventSubsytem::Get(this)->OnHintRequest.AddUniqueDynamic(this,&UPDWJigsawComponent::ShowHint);
}

void UPDWJigsawComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(UPDWPuzzlePieceComponent* PuzzlePiece = Cast<UPDWPuzzlePieceComponent>(OtherComp))
	{
		UPDWEventSubsytem::Get(this)->OnResetHintTimerRequestEvent();
		if(PuzzlePiece->GetID() == PuzzleID)
		{
			
			//Check Piece Rotation 
			if(!PuzzlePiece->GetComponentRotation().Equals(GetComponentRotation(),5.0f))
			{
				OverlappingPieces.AddUnique(PuzzlePiece);
				PuzzlePiece->OnOrientationChanged.AddUniqueDynamic(this, &UPDWJigsawComponent::CheckOrientation);
				return;
			}

			PuzzlePiece->SetWorldLocationAndRotation(GetComponentLocation(),GetComponentRotation());
			PuzzlePiece->SetSimulatePhysics(false);
			PuzzlePiece->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PuzzlePiece->SetEnableGravity(false);
			PuzzlePiece->SetComponentTickEnabled(false);
			PuzzlePiece->UnpossesComponent();
			//turning off his own collision.
			SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UFMODBlueprintStatics::PlayEvent2D(this,AudioEvent,true);
			UPDWEventSubsytem::Get(this)->OnTargetDeactivationEvent(GetOwner());
			APawn* CollidingPlayer = Cast<APawn>(OtherActor);
			if (CollidingPlayer)
			{
				UPDWGameplayFunctionLibrary::PlayForceFeedback({Cast<APDWPlayerController>(CollidingPlayer->GetController())}, UPDWGameplayTagSettings::GetTrunkImpactTag());	
			}

			if (bNotifySuccess)
			{
				FInteractionEventSignature InteractionEvent;
				InteractionEvent.EventTag = EventID;
				InteractionEvent.Interacter = OtherActor;
				UPDWEventSubsytem::Get(this)->TriggerInteractionEvent(TargetIDToNotify, InteractionEvent);
			}
		}
	}
}

void UPDWJigsawComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (UPDWPuzzlePieceComponent* PuzzlePiece = Cast<UPDWPuzzlePieceComponent>(OtherComp))
	{
		if(OverlappingPieces.Contains(PuzzlePiece))
		{
			OverlappingPieces.Remove(PuzzlePiece);
			PuzzlePiece->OnOrientationChanged.RemoveDynamic(this, &UPDWJigsawComponent::CheckOrientation);
		}
	}
}

void UPDWJigsawComponent::CheckOrientation(UPDWPuzzlePieceComponent* inComp)
{
	if (inComp->GetComponentRotation().Equals(GetComponentRotation(), 5.0f))
	{
		inComp->SetWorldLocationAndRotation(GetComponentLocation(), GetComponentRotation());
		inComp->SetSimulatePhysics(false);
		inComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		inComp->SetEnableGravity(false);
		inComp->SetComponentTickEnabled(false);
		inComp->OnOrientationChanged.RemoveDynamic(this, &UPDWJigsawComponent::CheckOrientation);
		inComp->UnpossesComponent();
		OverlappingPieces.Remove(inComp);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UPDWEventSubsytem::Get(this)->OnTargetDeactivationEvent(GetOwner());
		UFMODBlueprintStatics::PlayEvent2D(this,AudioEvent,true);
		if (bNotifySuccess)
		{
			FInteractionEventSignature InteractionEvent;
			InteractionEvent.EventTag = EventID;
			InteractionEvent.Interacter = nullptr; //doesn't really matter who triggered it at this point
			UPDWEventSubsytem::Get(this)->TriggerInteractionEvent(TargetIDToNotify, InteractionEvent);
		}

	}
}

void UPDWJigsawComponent::ShowHint(const FGameplayTag& inMinigameID)
{
	if(APDWMinigame_Base* Minigame = Cast<APDWMinigame_Base>(GetOwner()))
	{
		if(Minigame->GetMinigameConfigComp())
		{
			if(Minigame->GetMinigameConfigComp()->GetMinigameConfigurationData())
			{
				for (UPDWMinigameMovingMesh* Mesh : Minigame->GivenMeshes)
				{
					if (Mesh->GetPossessed())
					{
						UPDWPuzzlePieceComponent* PuzzlePiece = Cast<UPDWPuzzlePieceComponent>(Mesh);
						if (PuzzlePiece)
						{
							if (PuzzlePiece->GetID() == PuzzleID)
							{
								UPDWEventSubsytem::Get(this)->OnShowHintEvent(this,inMinigameID);
							}
						}
					}
				}
			}
		}
	}
}

void UPDWJigsawComponent::EndPlay(EEndPlayReason::Type Reason)
{
	UPDWEventSubsytem::Get(this)->OnHintRequest.RemoveDynamic(this,&UPDWJigsawComponent::ShowHint);
	OnComponentBeginOverlap.RemoveDynamic(this, &UPDWJigsawComponent::OnBeginOverlap);
	OnComponentEndOverlap.RemoveDynamic(this, &UPDWJigsawComponent::OnEndOverlap);
	Super::EndPlay(Reason);
}
