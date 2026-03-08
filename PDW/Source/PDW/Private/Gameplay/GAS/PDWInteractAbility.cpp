// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWInteractAbility.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "NebulaVehicle/PDWVehiclePawn.h"

UPDWInteractAbility::UPDWInteractAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MontageSettings.bActivateSkillState = false;
}

void UPDWInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle,ActorInfo,ActivationInfo,TriggerEventData);
	UPDWInteractionComponent* InteractionComponentToUse = nullptr;
	if (PlayerController->GetIsOnVehicle())
	{
		InteractionComponentToUse = PlayerController->GetVehicleInstance()->GetInteractionComponent();
	}
	else
	{
		InteractionComponentToUse = PlayerController->GetPupInstance()->GetInteractionComponent();
	}
	bool bIsAutoSwap = false;	
	if (!InteractionComponentToUse->InteractAction(bIsAutoSwap,EInteractionType::InputReceived))
	{
		if (!bIsAutoSwap)
		{
			//I'LL TRUGGER THE SKILL INTERACTION since is same button.
			PlayerController->OnInteractFailed();
		}
	}
}
