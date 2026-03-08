// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWGameplayHUD.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/HUD/PDWCharacterSelector.h"

void UPDWGameplayHUD::InitHUD(const TArray<APlayerController*>& Players)
{
	Super::InitHUD(Players);
}

void UPDWGameplayHUD::ManageRequestCharacterSelector(APlayerController* Player, bool Open, bool WithSelection)
{
	if (UPDWGameplayFunctionLibrary::IsSecondPlayer(Player))
	{
		if (Open)
		{
			Player2CharacterSelector->InitializeCharacterPicker(Player);
		}
		else
		{
			if (WithSelection)
			{
				Player2CharacterSelector->CloseWithSelection();
			}
			else
			{
				Player2CharacterSelector->CloseWithoutSelection();
			}
		}
	}
	else
	{
		if (Open)
		{
			Player1CharacterSelector->InitializeCharacterPicker(Player);
		}
		else
		{
			if (WithSelection)
			{
				Player1CharacterSelector->CloseWithSelection();
			}
			else
			{
				Player1CharacterSelector->CloseWithoutSelection();
			}
		}
	}

	OnOpenCharacterSelector(Player, Open);
}

void UPDWGameplayHUD::ManageEnterOverlappPen(FPDWDinoPenInfo PenInfo, APDWPlayerController* PlayerController)
{
	OnOverlappEnterPen(PenInfo, PlayerController);
}

void UPDWGameplayHUD::ManageExitOverlappPen(APDWPlayerController* PlayerController)
{
	OnOverlappExitPen(PlayerController);
}