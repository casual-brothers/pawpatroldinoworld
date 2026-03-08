// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWPupSwapBehaviour.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWPlayerState.h"

void UPDWPupSwapBehaviour::ExecuteBehaviour_Implementation()
{
	if (!PupTag.IsValid())
	{
		StopBehaviour();
		return;
	}
	IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(BehaviourExecutioner);
	if (Player)
	{
		APDWPlayerController* Controller = Cast<APDWPlayerController>(Player->GetPDWPlayerController());
		if (Controller->GetPDWPlayerState()->GetCurrentPup() != PupTag)
		{
			Controller->ChangeCharacter(PupTag);
		}
		if (bTurnIntoVehicle)
		{
			if (!Controller->GetIsOnVehicle())
			{
				Controller->ChangeVehicle(false, false);
			}
		}
	}
	StopBehaviour();
}