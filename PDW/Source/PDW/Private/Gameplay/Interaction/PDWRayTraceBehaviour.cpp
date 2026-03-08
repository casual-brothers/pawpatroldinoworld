// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWRayTraceBehaviour.h"
#include "Kismet/KismetSystemLibrary.h"

void UPDWRayTraceBehaviour::TickBehaviour_Implementation(float DeltaTime)
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
		if (HitResult.bBlockingHit)
		{
#if WITH_EDITOR
			if (TraceConfiguration.bDebug)
			{
				DrawDebugSphere(OwnerComponent->GetWorld(), HitResult.ImpactPoint, 20, 32, FColor::Green);
			}
#endif
			if (TraceConfiguration.bSphereTrace)
			{
				if (MultiHit)
				{
					OnRayTraceHits(HitsResult,DeltaTime);
					OnRayTraceHits_Implementation(HitsResult,DeltaTime);
				}
			}
			else
			{
				if (HitResult.GetComponent())
				{
					OnRayTraceHit(HitResult,DeltaTime);
					OnRayTraceHit_Implementation(HitResult, DeltaTime);
				}
			}
		}
#if WITH_EDITOR
		
		if (TraceConfiguration.bDebug)
		{
			DrawDebugLine(OwnerComponent->GetWorld(), StartTrace, EndTrace, FColor::Red, false, -1.0f, 0.0f, 3.0f);
		}
#endif
	}
}

void UPDWRayTraceBehaviour::InitBehaviour_Implementation(USceneComponent* NewOwnerComponent)
{
	Super::InitBehaviour_Implementation(NewOwnerComponent);
	PrepareMaterialInstanceDynamic(NewOwnerComponent->GetOwner());
	CachedFrameToSkipSettings = FrameToSkip;
	bCalculate = true;
}

void UPDWRayTraceBehaviour::OnRayTraceHit_Implementation(const FHitResult& inHitResult,float DeltaTime)
{
	if(MaterialConfiguration.TargetMaterialValue == -1)
		return;
	MaterialValue += DeltaTime;
	float CurrentValue = -1.0f;

	if (CachedMaterialInstanceDynamic != nullptr)
	{
		CachedMaterialInstanceDynamic->SetScalarParameterValueByInfo(MaterialConfiguration.MaterialInfo, MaterialValue);
		CachedMaterialInstanceDynamic->GetScalarParameterValue(MaterialConfiguration.MaterialInfo, CurrentValue);
	}

	if (CurrentValue >= MaterialConfiguration.TargetMaterialValue)
	{
		bCalculate = false;
		NotifySuccess();
	}
}

void UPDWRayTraceBehaviour::OnRayTraceHits_Implementation(const TArray<FHitResult>& Hits, float DeltaTime)
{

}

UMaterialInstanceDynamic* UPDWRayTraceBehaviour::GetMaterialInstanceDynamic()
{
	return CachedMaterialInstanceDynamic.Get();
}

void UPDWRayTraceBehaviour::SetMaterialInstanceDynamic(UMaterialInstanceDynamic* inInstance)
{
	CachedMaterialInstanceDynamic = inInstance;
}

UMeshComponent* UPDWRayTraceBehaviour::GetCachedMeshComponent()
{
	return CachedMeshComp.Get();
}

void UPDWRayTraceBehaviour::PrepareMaterialInstanceDynamic(AActor* inMinigameActor)
{
	if (inMinigameActor)
	{
		UMeshComponent* Comp = inMinigameActor->FindComponentByTag<UMeshComponent>(MaterialConfiguration.MeshComponentTag);
		if (Comp)
		{
			CachedMeshComp = Comp;
			const int32 MaterialIndex = CachedMeshComp->GetMaterialIndex(MaterialConfiguration.MaterialSlotNameToTrace);
			if (MaterialIndex != INDEX_NONE)
			{
				CachedMaterialInstanceDynamic = CachedMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, CachedMeshComp->GetMaterial(MaterialIndex));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("rAYtRACEbEHAVIOUR: MISSING MINIGAME ACTOR"));
	}
}
