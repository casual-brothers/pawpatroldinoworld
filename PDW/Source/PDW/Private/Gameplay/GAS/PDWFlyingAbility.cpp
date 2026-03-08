// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWFlyingAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Components/NebulaGameplaySurfaceControlComponent.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "Data/PDWGameplayTagSettings.h"

void UPDWFlyingAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    FlyingActor = Cast<AActor>(PlayerController->GetPawn());
    if (APDWCharacter* Char = Cast<APDWCharacter>(FlyingActor))
    {
        Char->bFlying = true;
    }
	UCharacterMovementComponent* MovComp = FlyingActor->FindComponentByClass<UCharacterMovementComponent>();
    MovComp->Velocity = FVector::ZeroVector;
    MovComp->GravityScale = 0;
    MovComp->JumpZVelocity = 0;
	MovComp->SetMovementMode(EMovementMode::MOVE_Falling);
	FlyingActor->FindComponentByClass<UNebulaGameplaySurfaceControlComponent>()->ActivateSurfaceComponent(false);
    USkeletalMeshComponent* PupMeshComp = FlyingActor->FindComponentByTag<USkeletalMeshComponent>(FGameplayTag::RequestGameplayTag("ComponentTag.CharacterMesh").GetTagName());
    if (PupMeshComp)
    {
       AnimInstance = Cast<UPDWAnimInstance>(PupMeshComp->GetAnimInstance());
       if (AnimInstance)
       {
           AnimInstance->SetAnimationState(EAnimationState::Flying);
       }
    }

    WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,UPDWGameplayTagSettings::GetAutoSwapRefreshTag());
	WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEnterAutoSwapArea);
	WaitEventTask->ReadyForActivation();
    WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,UPDWGameplayTagSettings::GetAutoSwapStopRefreshTag());
	WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnExitAutoSwapArea);
	WaitEventTask->ReadyForActivation();


}

void UPDWFlyingAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FlyingLogic(DeltaTime);
    DescendLogic(DeltaTime);
}

void UPDWFlyingAbility::FlyingLogic(const float DeltaTime)
{
	FVector ActorLocation = FlyingActor->GetActorLocation();

    const FVector Start = ActorLocation;
    const FVector End = Start - FVector(0, 0, DesiredFlyingAltitude/*+ 300.0f*/);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(FlyingActor);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, CollisionChannel, Params))
    {
        if (bIsDescending)
        {
            bIsDescending = false;
            if (bInputReleased && !bAutoRefresh)
            {
                CancelFlyght();
                EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
            }
        }
        const float TargetZ = HitResult.Location.Z + DesiredFlyingAltitude;
        const float NewZ = FMath::FInterpTo(ActorLocation.Z, TargetZ, DeltaTime, FlyingAltitudeSmoothVelocity);
        ActorLocation.Z = NewZ;
        FlyingActor->SetActorLocation(ActorLocation);
    }
    else
    {
        if (!bTimerRunning)
        {
            bTimerRunning = true;
            GetWorld()->GetTimerManager().SetTimer(FlyingTimerHandle,this,&UPDWFlyingAbility::OnTimerFinished, FlyingDuration, false);
        }
    }

	if (bDebug)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 30, 32, FColor::Red);
	}
}

void UPDWFlyingAbility::CancelFlyght()
{
    TransitionToExitAnimation();
    APDWCharacter* Char = Cast<APDWCharacter>(FlyingActor);
    Char->bFlying = false;
    UCharacterMovementComponent* MovComp = FlyingActor->FindComponentByClass<UCharacterMovementComponent>();
    if (MovComp)
    {
        MovComp->GravityScale = Char->GetCachedGravityScale();
        MovComp->JumpZVelocity = 700;
    }
    FlyingActor->FindComponentByClass<UNebulaGameplaySurfaceControlComponent>()->ActivateSurfaceComponent(true);
    MovComp->SetMovementMode(EMovementMode::MOVE_Walking);
    if (AnimInstance)
    {
        AnimInstance->SetAnimationState(EAnimationState::Walking);
    }
}

void UPDWFlyingAbility::DescendLogic(const float DeltaTime)
{
	if (!bIsDescending)
	{
		return;
	}

	FVector ActorLocation = FlyingActor->GetActorLocation();
    
    const float TargetZ = ActorLocation.Z - FallSpeed;
    const float NewZ = FMath::FInterpTo(ActorLocation.Z, TargetZ, DeltaTime, FlyingDescendingSmoothVelocity); 
    ActorLocation.Z = NewZ;

    FlyingActor->SetActorLocation(ActorLocation);
}

void UPDWFlyingAbility::OnTimerFinished()
{
    bTimerRunning = false;
	bIsDescending = true;
}

void UPDWFlyingAbility::OnReleaseInput(FGameplayEventData inPayLoad)
{
	Super::OnReleaseInput(inPayLoad);
    if (!bTimerRunning && !bAutoRefresh)
	{
        if (PupBackPackAnimInstance)
	    {
		PupBackPackAnimInstance->Montage_SetNextSection(MontageSettings.SectionToLoop,MontageSettings.SectionToLoop);
	    }
		bTimerRunning = true;
		GetWorld()->GetTimerManager().SetTimer(FlyingTimerHandle, this, &UPDWFlyingAbility::OnTimerFinished, FlyingDuration, false);
	}
}

void UPDWFlyingAbility::TransitionToExitAnimation()
{
	if (PupBackPackAnimInstance)
	{
		PupBackPackAnimInstance->Montage_JumpToSection(StopFlySection);
	}
}

void UPDWFlyingAbility::OnPupSwap(APDWPlayerController* inController)
{
    bForceCancelAbility = true;
}

void UPDWFlyingAbility::OnEnterAutoSwapArea(FGameplayEventData inPayLoad)
{
    if (inPayLoad.Target == PlayerController)
    {
        PlayerController->OnChangeCharacterRequested.AddUniqueDynamic(this,&UPDWFlyingAbility::OnPupSwap);
        bAutoRefresh = true;
        if (PupBackPackAnimInstance)
		{
            PupBackPackAnimInstance->Montage_SetNextSection(MontageSettings.SectionToLoop,MontageSettings.SectionToLoop);
		}
    }
}

void UPDWFlyingAbility::OnExitAutoSwapArea(FGameplayEventData inPayLoad)
{
    if (inPayLoad.Target == PlayerController)
    {
        PlayerController->OnChangeCharacterRequested.RemoveDynamic(this,&UPDWFlyingAbility::OnPupSwap);
        bAutoRefresh = false;
  //      if (PupBackPackAnimInstance)
		//{
  //          PupBackPackAnimInstance->Montage_SetNextSection(MontageSettings.SectionToLoop,MontageSettings.SectionToLoop);
		//}
    }
}

void UPDWFlyingAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    CancelFlyght();
    bForceCancelAbility = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPDWFlyingAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    if(bAutoRefresh && !bForceCancelAbility)
        return;
    CancelFlyght();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
