// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWSplineRampComponent.h"
#include "Managers/PDWEventSubsytem.h"

UPDWSplineRampComponent::UPDWSplineRampComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPDWSplineRampComponent::MoveOnSpline(const float inDeltaTime)
{
	if (bLogicEnabled && RampUser)
	{
		CurrentDistance += inDeltaTime;
		
		FTransform PositionOnSpline = GetTransformAtDistanceAlongSpline(CurrentDistance * RampSpeed, ESplineCoordinateSpace::World);
		const FVector Location = PositionOnSpline.GetLocation() + FVector::UpVector * JumpValue;
		RampUser->SetActorLocationAndRotation(Location, PositionOnSpline.GetRotation());
		
		if (CurrentDistance >= GetSplineLength())
		{
			bLogicEnabled = false;
		}
	}
	else
	{
		CurrentDistance = 0;
	}
}

void UPDWSplineRampComponent::SetLogicEnabled(bool bInActive)
{
	bLogicEnabled = bInActive;
}

void UPDWSplineRampComponent::SetRampUser(AActor* inActor)
{
	RampUser = inActor;
}

bool UPDWSplineRampComponent::GetLogicEnabled() const
{
	return bLogicEnabled;
}

AActor* UPDWSplineRampComponent::GetRampUser() const
{
	return RampUser;
}

void UPDWSplineRampComponent::Jump(const float& inJumpValue)
{
	JumpValue = inJumpValue;
}

void UPDWSplineRampComponent::StopJump()
{
	JumpValue = 0.0f;
}

void UPDWSplineRampComponent::StopRampUsage(AActor* Actor)
{
	SetLogicEnabled(false);
}

void UPDWSplineRampComponent::BeginPlay()
{
	Super::BeginPlay();
	UPDWEventSubsytem::Get(this)->OnJumpEventOnSpline.AddUniqueDynamic(this, &UPDWSplineRampComponent::Jump);
	UPDWEventSubsytem::Get(this)->OnStopJumpEvent.AddUniqueDynamic(this, &UPDWSplineRampComponent::StopJump);
	UPDWEventSubsytem::Get(this)->OnMinigameCompleteOrLeft.AddUniqueDynamic(this, &UPDWSplineRampComponent::StopRampUsage);
}

void UPDWSplineRampComponent::EndPlay(EEndPlayReason::Type Reason)
{
	UPDWEventSubsytem::Get(this)->OnJumpEventOnSpline.RemoveAll(this);
	UPDWEventSubsytem::Get(this)->OnStopJumpEvent.RemoveAll(this);
	UPDWEventSubsytem::Get(this)->OnMinigameCompleteOrLeft.RemoveAll(this);
	
	Super::EndPlay(Reason);
}
