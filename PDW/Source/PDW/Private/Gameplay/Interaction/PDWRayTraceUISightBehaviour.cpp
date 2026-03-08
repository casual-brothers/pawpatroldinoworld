// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWRayTraceUISightBehaviour.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "UI/Widgets/PDWSightWidget.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Gameplay/Components/PDWTargetingComponent.h"

void UPDWRayTraceUISightBehaviour::OnRayTraceHit_Implementation(const FHitResult& inHitResult, float DeltaTime)
{
	// #DEV <why the hell this logic is in a ray trace considering that once the object is locked the stuff doesn't move.> [#daniele.m, 18 December 2025, OnRayTraceHit_Implementation]
	Super::OnRayTraceHit_Implementation(inHitResult, DeltaTime);
	
	FVector3d OwnerPosition = GetOwnerComponent()->GetComponentLocation();
	FVector3d OwnerForward = GetOwnerComponent()->GetForwardVector();

	FVector3d WorldPos = OwnerPosition + (OwnerForward * DistanceFromActor);

	MoveSight(inHitResult, WorldPos);

#if WITH_EDITOR
	if (TraceConfiguration.bDebug)
	{
		DrawDebugSphere(OwnerComponent->GetWorld(), WorldPos, 20, 32, FColor::Cyan);
	}
#endif

}

void UPDWRayTraceUISightBehaviour::OnRayTraceHits_Implementation(const TArray<FHitResult>& Hits, float DeltaTime)
{
	Super::OnRayTraceHits_Implementation(Hits, DeltaTime);

	FVector3d OwnerPosition = GetOwnerComponent()->GetComponentLocation();
	FVector3d OwnerForward = GetOwnerComponent()->GetForwardVector();

	FVector3d WorldPos = OwnerPosition + (OwnerForward * DistanceFromActor);

	MoveSight(Hits.Num()? Hits[0] : FHitResult(), WorldPos);

#if WITH_EDITOR
	if (TraceConfiguration.bDebug)
	{
		DrawDebugSphere(OwnerComponent->GetWorld(), WorldPos, 20, 32, FColor::Cyan);
	}
#endif
}

void UPDWRayTraceUISightBehaviour::InitBehaviour_Implementation(USceneComponent* NewOwnerComponent)
{
	Super::InitBehaviour_Implementation(NewOwnerComponent);
	APDWPlayerController* OwnerPlayerController = Cast<APDWPlayerController>(BehaviourExecutioner);
	SightWidget = UPDWHUDSubsystem::Get(this)->CreateSight(SightWidgetClass, OwnerPlayerController);
}

void UPDWRayTraceUISightBehaviour::StopBehaviour_Implementation()
{
	APDWPlayerController* OwnerPlayerController = Cast<APDWPlayerController>(BehaviourExecutioner);
	UPDWHUDSubsystem::Get(this)->RemoveSight(OwnerPlayerController);
	Super::StopBehaviour_Implementation();
}

void UPDWRayTraceUISightBehaviour::MoveSight(const FHitResult& inHitResult, FVector3d WorldPos)
{
	if(!SightWidget) return;
	FVector2D ScreenPos;
	UGameplayStatics::ProjectWorldToScreen(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this), WorldPos, ScreenPos);
	ScreenPos = FVector2D(ScreenPos.X - (SightWidget->GetDesiredSize().X/2),ScreenPos.Y - (SightWidget->GetDesiredSize().Y/2));
	SightWidget->SetPositionInViewport(ScreenPos);
	if(inHitResult.GetComponent())
	{
		if (UPDWTargetingComponent* TargetingComp = Cast<UPDWTargetingComponent>(inHitResult.GetComponent()))
		{
			SightWidget->FoundTarget(TargetingComp->GetIsTargeted());
		}
		else
		{
			SightWidget->FoundTarget(false);
		}
	}
	else
	{
		SightWidget->FoundTarget(false);
	}
}