// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWPickUpComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

UPDWPickUpComponent::UPDWPickUpComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	BodyInstance.SetCollisionProfileName("OverlapOnlyPlayer");
}

void UPDWPickUpComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	ActorOwner = GetOwner();

	OnComponentBeginOverlap.AddDynamic(this, &UPDWPickUpComponent::OnOverlapBegin);
}

void UPDWPickUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	if (DoingPickUpAnimation)
	{
		PickUpAnimation(DeltaTime);
	}
}

void UPDWPickUpComponent::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	StartPickUp(OtherActor);
}

void UPDWPickUpComponent::StartPickUp(AActor* OverlappingCharacter)
{
	if (OverlappingCharacter)
	{
		PickUpTarget = OverlappingCharacter;
		IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(PickUpTarget);
		if (!Player) return;
		
		PickUpTargetController = Player->GetPDWPlayerController();
		if (!PickUpTargetController)
		{
			//for Timmy
			APDWCharacter* PickUpCharacter = Cast<APDWCharacter>(PickUpTarget);
			if (PickUpCharacter)
			{
				PickUpTargetController = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
			}
		}

		DoingPickUpAnimation = true;
		CurrentAnimationTime = 0.f;
		SetComponentTickEnabled(true);
		OnPickUpStart.Broadcast(this, Cast<APDWPlayerController>(Player->GetPDWPlayerController()));
		OnComponentBeginOverlap.RemoveDynamic(this, &UPDWPickUpComponent::OnOverlapBegin);
	}
}

void UPDWPickUpComponent::PickUpAnimation(float DeltaTime)
{
	if (!MagneticCurve)
	{
		EndPickUp();
		return;
	}

	float CurrentSpeed = MagneticCurve->GetFloatValue(CurrentAnimationTime);
	CurrentAnimationTime += DeltaTime;

	if (ActorOwner && PickUpTarget)
	{
		FVector OwnerLocation = ActorOwner->GetActorLocation();
		FVector TargetLocation = PickUpTarget->GetActorLocation();
		ActorOwner->SetActorLocation(FMath::VInterpConstantTo(OwnerLocation, TargetLocation, DeltaTime, CurrentSpeed));
		OwnerLocation = ActorOwner->GetActorLocation();

		float CurrentDistance = FVector::Dist(OwnerLocation, TargetLocation);

		if (CurrentDistance <= PickUpDistance)
		{
			EndPickUp();
		}
	}
	else
	{
		EndPickUp();
	}
}

void UPDWPickUpComponent::EndPickUp()
{
	if (PickUpTarget)
	{
		OnPickUpEnd.Broadcast(this, PickUpTargetController);
	}
	else
	{
		OnPickUpEnd.Broadcast(this, nullptr);
	}

	PickUpTarget = nullptr;
	DoingPickUpAnimation = false;
}

void UPDWPickUpComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OnComponentBeginOverlap.IsBound())
	{
		OnComponentBeginOverlap.RemoveDynamic(this, &UPDWPickUpComponent::OnOverlapBegin);
	}

	Super::EndPlay(EndPlayReason);
}
