// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWPreyComponent.h"
#include "Components/SplineComponent.h"

// Sets default values for this component's properties
UPDWPreyComponent::UPDWPreyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPDWPreyComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UPDWPreyComponent::MoveAlongSpline(float DeltaTime)
{
    if (!PlayerPawn || !GetOwner() || !Spline || bPauseLogic)
    {
        return;
    }

	const FVector ActorToPlayer = GetOwner()->GetActorLocation() - PlayerPawn->GetActorLocation();
	const float ForwardDistance = FVector::DotProduct(ActorToPlayer, GetOwner()->GetActorForwardVector());

	// Define target speed
	float TargetSpeed;

	if (ForwardDistance < PreyConfig.MinDistanceFromPlayer)
	{
		TargetSpeed =  PreyConfig.MaxSpeed;
	}
	else if (ForwardDistance >PreyConfig.MaxDistanceFromPlayer)
	{
		TargetSpeed = PreyConfig.MinSpeed; 
	}
	else
	{
		float Alpha = (ForwardDistance - PreyConfig.MinDistanceFromPlayer) / (PreyConfig.MaxDistanceFromPlayer - PreyConfig.MinDistanceFromPlayer);
		TargetSpeed = FMath::Lerp(PreyConfig.MaxSpeed, PreyConfig.MinSpeed, Alpha);
	}

    CurrentSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, PreyConfig.SpeedAdjustRate);

    //GEngine->AddOnScreenDebugMessage(348374,0,FColor::Red,FString::Printf(TEXT("Speed : %f"),CurrentSpeed));
    //GEngine->AddOnScreenDebugMessage(3483374,0,FColor::Red,FString::Printf(TEXT("Distance : %f"),ForwardDistance));
    DistanceAlongSpline += CurrentSpeed * DeltaTime;
    FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	const FRotator NewRotation = Spline->GetWorldRotationAtDistanceAlongSpline(DistanceAlongSpline);
    GetOwner()->SetActorLocationAndRotation(NewLocation,NewRotation);
}

// Called every frame
void UPDWPreyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveAlongSpline(DeltaTime);
}

void UPDWPreyComponent::SetPlayerPawn(APawn* inPawn)
{
	PlayerPawn = inPawn;
}

void UPDWPreyComponent::SetSpline(USplineComponent* inSpline)
{
    Spline =  inSpline;
}

void UPDWPreyComponent::PauseLogic(bool bInPause)
{
	bPauseLogic = bInPause;
}

