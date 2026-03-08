// Fill out your copyright notice in the Description page of Project Settings.
#include "Gameplay/GAS/PDWGAS_SwapCharacter.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "NebulaVehicle/PDWVehiclePawn.h"

void UPDWGAS_SwapCharacter::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (PlayerState)
	{
		if (PlayerState->GetIsOnVehicle())
		{
			PlayerController->OnPupCustomizationCompleted.AddDynamic(this, &UPDWGAS_SwapCharacter::OnSwapCompleted);
			SwapCharacterCustomization(true,EPupRequestType::Current);
		}
		else
		{
			SwapCharacterCustomization(false,EPupRequestType::Next);
			CompleteCharacterSwap();
		}
	}
}

void UPDWGAS_SwapCharacter::OnSwapCompleted(APDWPlayerController* inController)
{
	inController->OnPupCustomizationCompleted.RemoveDynamic(this, &UPDWGAS_SwapCharacter::OnSwapCompleted);
	if (PlayerState->GetIsOnVehicle())
	{
		inController->ChangeVehicle(true, false);
	}
	CompleteCharacterSwap();
}
