// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWRenderTargetBehaviour.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "UI/Widgets/PDWSightWidget.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Gameplay/Components/PDWTargetingComponent.h"

void UPDWRenderTargetBehaviour::Paint(const FHitResult& inHit)
{
	BP_Paint(inHit);
}

void UPDWRenderTargetBehaviour::InitBehaviour_Implementation(USceneComponent* NewOwnerComponent)
{
	Super::InitBehaviour_Implementation(NewOwnerComponent);
	bCalculate = true;
	APDWPlayerController* OwnerPlayerController = Cast<APDWPlayerController>(BehaviourExecutioner);
	SightWidget = UPDWHUDSubsystem::Get(this)->CreateSight(SightWidgetClass, OwnerPlayerController);
}

void UPDWRenderTargetBehaviour::TickBehaviour_Implementation(float DeltaTime)
{
	if(!bCalculate)
		return;

	const FVector StartTrace = OwnerComponent->GetComponentLocation();
	const FVector EndTrace = StartTrace + (OwnerComponent->GetForwardVector() * TraceConfiguration.TraceLength);
	FHitResult HitResult;
	OwnerComponent->GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, TraceConfiguration.CollisionChannel);
	MoveSight(HitResult, HitResult.bBlockingHit ? HitResult.ImpactPoint : EndTrace);
	if (HitResult.bBlockingHit)
	{
#if WITH_EDITOR
		if (TraceConfiguration.bDebug)
		{
			DrawDebugSphere(OwnerComponent->GetWorld(), HitResult.ImpactPoint, 20, 32, FColor::Green);
		}
#endif
	}
	Paint(HitResult);
#if WITH_EDITOR
		
	if (TraceConfiguration.bDebug)
	{
		DrawDebugLine(OwnerComponent->GetWorld(), StartTrace, EndTrace, FColor::Red, false, -1.0f, 0.0f, 3.0f);
	}
#endif
}

void UPDWRenderTargetBehaviour::MoveSight(const FHitResult& inHitResult, FVector3d WorldPos)
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

void UPDWRenderTargetBehaviour::StopBehaviour_Implementation()
{
	APDWPlayerController* OwnerPlayerController = Cast<APDWPlayerController>(BehaviourExecutioner);
	UPDWHUDSubsystem::Get(this)->RemoveSight(OwnerPlayerController);
	Super::StopBehaviour_Implementation();
}
