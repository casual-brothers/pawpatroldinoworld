// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWVehicleInteractionComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Gameplay/Pawns/PDWCharacter.h"

void UPDWVehicleInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		AllowedInteractions.AppendTags(Controller->GetPupInstance()->GetInteractionComponent()->GetAllowedInteractions());
	}
	else
	{
		IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(GetOwner());
		if (PlayerInterface)
		{
			PlayerInterface->OnPlayerPossesed.AddUniqueDynamic(this,&UPDWVehicleInteractionComponent::OnCharacterPossessed);
		}
	}
}

void UPDWVehicleInteractionComponent::OnCharacterPossessed()
{
	IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(GetOwner());
	if (PlayerInterface)
	{
		APDWPlayerController* Controller = PlayerInterface->GetPDWPlayerController();
		AllowedInteractions.AppendTags(Controller->GetPupInstance()->GetInteractionComponent()->GetAllowedInteractions());
		PlayerInterface->OnPlayerPossesed.RemoveDynamic(this, &UPDWVehicleInteractionComponent::OnCharacterPossessed);
	}
}

void UPDWVehicleInteractionComponent::RemoveAllowedInteraction(const FGameplayTagContainer& inContainer)
{
	Super::RemoveAllowedInteraction(inContainer);
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->GetPupInstance()->GetInteractionComponent()->RemoveAllowedInteraction(inContainer);
	}
}

void UPDWVehicleInteractionComponent::SetAllowedInteraction(const FGameplayTagContainer& inContainer)
{
	Super::SetAllowedInteraction(inContainer);
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->GetPupInstance()->GetInteractionComponent()->SetAllowedInteraction(inContainer);
	}
}
