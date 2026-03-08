// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWAIMoveComponent.h"
#include "Managers/PDWEventSubsytem.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "AITypes.h"
#include "AIController.h"
#include "Gameplay/Actors/PDWDinoPenArea.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

// Sets default values for this component's properties
UPDWAIMoveComponent::UPDWAIMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UPDWAIMoveComponent::Init(FGameplayTag _PenTag, APDWDinoPenArea* _PenAreaRef)
{
	if(!_PenAreaRef) return;

	AActor* OwnerActor = GetOwner();
	if(!OwnerActor) return;

	CharMoveComp = Cast<UCharacterMovementComponent>(OwnerActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
	if(!CharMoveComp) return;

	TArray<UActorComponent*> Components = OwnerActor->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), SkeletalToCheck.GetTagName());
	if (Components.Num() == 0)return;	
	USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(Components[0]);
	if (!SkeletalComp) return;
	AnimInstance = SkeletalComp->GetAnimInstance();	
	if(!AnimInstance) return;

	AnimInstance->OnMontageStarted.AddUniqueDynamic(this, &UPDWAIMoveComponent::OnMontageStart);
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UPDWAIMoveComponent::OnMontageEnd);

	NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if(!NavSys) return;

	OwnerPawn = Cast<APawn>(OwnerActor);
	PenTag = _PenTag;
	FPDWPenAreaBounds PenAreaBounds = _PenAreaRef->GetPenAreaBounds();
	PenCenter = PenAreaBounds.PenAreaCenter;
	PenRadius = PenAreaBounds.MaxMoveRadius;
	DefaultWalkSpeed = CharMoveComp->MaxWalkSpeed;

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnDinoPenActivityChange.AddUniqueDynamic(this, &UPDWAIMoveComponent::OnDinoPenActivityChange);
	}

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		IsPenActive = PaleoCenterSubsystem->GetDinoPenActivity(PenTag);
	}

	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle,this,&UPDWAIMoveComponent::Move, RandomMoveTimer, true);
}

// Called when the game starts
void UPDWAIMoveComponent::BeginPlay()
{
	Super::BeginPlay();

}


bool UPDWAIMoveComponent::CanMove()
{
	return IsPenActive && !IsDoingAMontage;
}

void UPDWAIMoveComponent::Move()
{
	if(!CanMove() || !OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if(!AIController) return;

	FNavLocation RandomLocation;
    bool bFound = NavSys->GetRandomReachablePointInRadius(PenCenter, PenRadius, RandomLocation);
	AIController->MoveToLocation(RandomLocation, AcceptanceRadius);
}

void UPDWAIMoveComponent::OnDinoPenActivityChange(const FGameplayTag _PenTag, bool IsActive)
{
	if (PenTag == _PenTag)
	{
		IsPenActive = IsActive;

		if (!IsPenActive)
		{
			AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
			AIController->StopMovement();
		}
	}
}

void UPDWAIMoveComponent::OnMontageStart(UAnimMontage* Montage)
{
	IsDoingAMontage = true;
	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	AIController->StopMovement();
}

void UPDWAIMoveComponent::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IsDoingAMontage = false;
}

void UPDWAIMoveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AnimInstance)
	{
		AnimInstance->OnMontageStarted.RemoveDynamic(this, &UPDWAIMoveComponent::OnMontageStart);
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UPDWAIMoveComponent::OnMontageEnd);
	}

	if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this))
	{
		EventSubsystem->OnDinoPenActivityChange.RemoveDynamic(this, &UPDWAIMoveComponent::OnDinoPenActivityChange);
	}

	Super::EndPlay(EndPlayReason);
}

void UPDWAIMoveComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	if(!CharMoveComp || !OwnerPawn) return;

	APDWPlayerController* Player1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if(!Player1) return;
	APawn* P1Pawn = Player1->GetPawn();
	if(!P1Pawn) return;

	float P1DistanceToCheck = Player1->GetIsOnVehicle() ? StopDistanceFromVehicles : StopDistanceFromPlayers;
	CharMoveComp->MaxWalkSpeed = FVector::Dist(P1Pawn->GetActorLocation(), OwnerPawn->GetActorLocation()) < P1DistanceToCheck ? 0.f : DefaultWalkSpeed;

	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* Player2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		if(!Player2) return;
		APawn* P2Pawn = Player2->GetPawn();
		if(!P2Pawn) return;

		float P2DistanceToCheck = Player2->GetIsOnVehicle() ? StopDistanceFromVehicles : StopDistanceFromPlayers;
		CharMoveComp->MaxWalkSpeed = FVector::Dist(P2Pawn->GetActorLocation(), OwnerPawn->GetActorLocation()) < P2DistanceToCheck ? 0.f : DefaultWalkSpeed;
	}
}

