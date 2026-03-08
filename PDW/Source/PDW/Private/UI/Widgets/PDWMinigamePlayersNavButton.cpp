// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWMinigamePlayersNavButton.h"
#include "UI/HUD/PDWMinigameHUD.h"
#include "Components/Image.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

void UPDWMinigamePlayersNavButton::InitMinigamePlayersButton(const FPDWMinigameInputData& InputData)
{
	NavButton->InitializeButton(InputData.ButtonData);
	if ((!UPDWGameplayFunctionLibrary::IsMultiplayerOn(this)) || InputData.InputPlayer == EPlayerInput::BothPlayers)
	{
		PlayerImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		PlayerImage->SetBrushFromSoftTexture(InputData.InputPlayer == EPlayerInput::Player1 ? Player1Texture :Player2Texture);
	}
}