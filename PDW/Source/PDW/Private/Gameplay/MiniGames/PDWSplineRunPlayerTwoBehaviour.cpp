// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWSplineRunPlayerTwoBehaviour.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraFunctionLibrary.h"

void UPDWSplineRunPlayerTwoBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	Super::InitializeBehaviour(inController, inMiniGameComp);
	FakeChase = inMiniGameComp->GetOwner()->FindComponentByTag<USkeletalMeshComponent>("FakeChase");
	CenterLocation = FakeChase.Get()->GetRelativeLocation();
	TargetRelativeLocation = CenterLocation;
	FakeChase->SetVisibility(true,true);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(FakeChase.Get(), SpawnVFX, FakeChase.Get()->GetComponentLocation());
}

void UPDWSplineRunPlayerTwoBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	if (bIsTransitioning)
	{
		return;
	}

	const FVector2D Value = inInputInstance.GetValue().Get<FVector2D>();
	
	if (Value.X == 0) 
	{
		return;
	}

	if (Value.X > 0 && CurrentSplineIndex == 1)
	{
		return;
	}
	if (Value.X < 0 && CurrentSplineIndex == -1)
	{
		return;
	}
	const int32 SelectedSplineIndex = (Value.X > 0 ? 1 : -1);
	int32 NewIndex = CurrentSplineIndex + SelectedSplineIndex;
	NewIndex = FMath::Clamp(NewIndex, -1, 1);
	bIsTransitioning = true;
	CurrentSplineIndex = NewIndex;

	Move(NewIndex);
}

void UPDWSplineRunPlayerTwoBehaviour::CustomTick(const float inDeltaTime)
{
	if(!bIsTransitioning || !FakeChase.Pin())
		return;

    FVector Current = FakeChase.Get()->GetRelativeLocation();
    FVector NewLocation = FMath::VInterpTo(Current,TargetRelativeLocation,inDeltaTime,LerpSpeed);

    FakeChase.Get()->SetRelativeLocation(NewLocation);

	float Tolerance = 0.3f;

	bool bReachedTarget =FVector::DistSquared(FakeChase.Get()->GetRelativeLocation(),TargetRelativeLocation) <= FMath::Square(Tolerance);
	if (bReachedTarget)
	{
		bIsTransitioning = false;
	}
}

void UPDWSplineRunPlayerTwoBehaviour::Move(int32 NewIndex)
{
	TargetRelativeLocation = CenterLocation +FVector(0,NewIndex * SideDistance,0);
}
