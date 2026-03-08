// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWBaseRayTraceBehaviour.h"
#include "Kismet/KismetSystemLibrary.h"

void UPDWBaseRayTraceBehaviour::OnRayTraceHit_Implementation(const FHitResult& inHitResult, float DeltaTime)
{
	
}

void UPDWBaseRayTraceBehaviour::InitBehaviour_Implementation(USceneComponent* NewOwnerComponent)
{
	CachedFrameToSkipSettings = FrameToSkip;
	bCalculate = true;
	Super::InitBehaviour_Implementation(NewOwnerComponent);
}

void UPDWBaseRayTraceBehaviour::OnRayTraceHits_Implementation(const TArray<FHitResult>& Hits, float DeltaTime)
{
}

void UPDWBaseRayTraceBehaviour::TickBehaviour_Implementation(float DeltaTime)
{
	if(!bCalculate)
		return;
	FrameToSkip--;
	if (FrameToSkip <= 0)
	{
		FrameToSkip = CachedFrameToSkipSettings;
		const FVector StartTrace = OwnerComponent->GetComponentLocation();
		const FVector EndTrace = StartTrace + (OwnerComponent->GetForwardVector() * TraceConfiguration.TraceLength);
		FHitResult HitResult;
		TArray<FHitResult> HitsResult;
		bool MultiHit = false;
		if (TraceConfiguration.bSphereTrace)
		{
			MultiHit = UKismetSystemLibrary::SphereTraceMulti(this,StartTrace,EndTrace,TraceConfiguration.TraceRadius,UEngineTypes::ConvertToTraceType(TraceConfiguration.CollisionChannel), false,{},TraceConfiguration.bDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,HitsResult,true);
		}
		else
		{
			OwnerComponent->GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, TraceConfiguration.CollisionChannel);
		}

#if WITH_EDITOR
		if (TraceConfiguration.bDebug)
		{
			DrawDebugSphere(OwnerComponent->GetWorld(), HitResult.ImpactPoint, 20, 32, FColor::Green);
		}
#endif

		if (TraceConfiguration.bSphereTrace)
		{
				OnRayTraceHits(HitsResult, DeltaTime);
				OnRayTraceHits_Implementation(HitsResult, DeltaTime);
			if (MultiHit)
			{
			}
		}
		else
		{
			OnRayTraceHit(HitResult, DeltaTime);
			OnRayTraceHit_Implementation(HitResult, DeltaTime);
		}
#if WITH_EDITOR
		
		if (TraceConfiguration.bDebug)
		{
			DrawDebugLine(OwnerComponent->GetWorld(), StartTrace, EndTrace, FColor::Red, false, -1.0f, 0.0f, 3.0f);
		}
#endif
	}
}
