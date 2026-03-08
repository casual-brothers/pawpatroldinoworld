// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Animation/PDWAnimInstance.h"
#include "Managers/PDWEventSubsytem.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Engine/StreamableManager.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Engine/AssetManager.h"

void UPDWAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnCutsceneStart.AddUniqueDynamic(this, &UPDWAnimInstance::HandleCutsceneStart);
			EventSubsystem->OnCutsceneEnd.AddUniqueDynamic(this, &UPDWAnimInstance::HandleCutsceneEnd);
		}
	}
}

bool UPDWAnimInstance::EnabledOnCurrentPlatform()
{
	return bLogicEnabledOnCurrentPlatform.GetDefault();
;}

void UPDWAnimInstance::SetAnimationState(const EAnimationState inNewState)
{
	CurrentAnimationState = inNewState;
}

void UPDWAnimInstance::SetDivingMode(const bool bInActive)
{
	bDiving = bInActive;
}

bool UPDWAnimInstance::GetSwimmingState() const
{
	return CurrentAnimationState == EAnimationState::Swimming;
}

bool UPDWAnimInstance::GetDrivingState() const
{
	return CurrentAnimationState == EAnimationState::Driving;
}

bool UPDWAnimInstance::GetDivingState() const
{
	return bDiving;
}

bool UPDWAnimInstance::GetFlyingState() const
{
	return CurrentAnimationState == EAnimationState::Flying;
}

bool UPDWAnimInstance::GetSkillState() const
{
	return CurrentAnimationState == EAnimationState::Skill;
}

bool UPDWAnimInstance::GetSniffState() const
{
	return CurrentAnimationState == EAnimationState::Sniff;
}

bool UPDWAnimInstance::GetCutsceneLoopAnimation() const
{
	return AnimationToLoopForCutscene ? true : false;
}

bool UPDWAnimInstance::GetIsInCutscene() const
{
	return bIsInCutscene;
}

bool UPDWAnimInstance::PlayMontageByTag(FGameplayTag MontageTag)
{
	if (AvailableMontages.Contains(MontageTag))
	{
		TWeakObjectPtr<UPDWAnimInstance> WeakThis(this);

		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(AvailableMontages[MontageTag].ToSoftObjectPath(),[WeakThis,MontageTag]{
		WeakThis->Montage_Play(WeakThis->AvailableMontages[MontageTag].Get());
		},FStreamableManager::AsyncLoadHighPriority);
		//Montage_Play(AvailableMontages[MontageTag].LoadSynchronous());
		return true;
	}
	else
	{
		return false;
	}
}

TSoftObjectPtr<UAnimMontage> UPDWAnimInstance::GetMontageByTag(FGameplayTag MontageTag)
{
	if (AvailableMontages.Contains(MontageTag))
	{
		return AvailableMontages[MontageTag];
	}
	else
	{
		return nullptr;
	}
}

const EAnimationState UPDWAnimInstance::GetCurrentAnimationState() const
{
	return CurrentAnimationState;
}

void UPDWAnimInstance::BeginDestroy()
{
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnCutsceneStart.RemoveDynamic(this, &UPDWAnimInstance::HandleCutsceneStart);
			EventSubsystem->OnCutsceneEnd.RemoveDynamic(this, &UPDWAnimInstance::HandleCutsceneEnd);
		}
	}

	Super::BeginDestroy();
}