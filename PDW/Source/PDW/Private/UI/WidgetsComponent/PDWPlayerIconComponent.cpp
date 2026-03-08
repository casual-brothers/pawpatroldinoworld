// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetsComponent/PDWPlayerIconComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

void UPDWPlayerIconComponent::HandleMultiplayerVisibility()
{
	if(!GetOwner()) return;
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if(!PawnOwner) return;

	if (!UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		SetHiddenInGame(true);
		StayHidden = true;
	}
	else
	{
		SetHiddenInGame(false);
		StayHidden = false;
		AController* MyController = Cast<AController>(PawnOwner->GetController());
		if (MyController == UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this))
		{
			SetOwnerPlayer(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this)->GetLocalPlayer());
		}
		else
		{
			SetOwnerPlayer(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->GetLocalPlayer());
		}
	}	
}
