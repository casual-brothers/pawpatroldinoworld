// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWSightWidget.h"
#include "Gameplay/MiniGames/PDWButtonSequenceInputBehaviour.h"
#include "Data/PDWGameSettings.h"
#include "UI/Widgets/PDWMinigameNavButton.h"
#include "Managers/PDWHUDSubsystem.h"
#include "UI/Widgets/PDWPlayerIndicator.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

void UPDWSightWidget::FoundTarget(bool Found)
{
	bIsTargetFound = Found;
	BP_FoundTarget(Found);
}

void UPDWSightWidget::StartMinigameInputActionIcon(FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack /*= FNavButtonTriggerCallback()*/, FNavButtonProgressCallback InProgressCallBack /*= FNavButtonProgressCallback()*/)
{
	if (!MinigameNavButton) return;

	const FButtonSequenceConfiguration* InputSequenceConfig = InputActionToShow.GetPtr<FButtonSequenceConfiguration>();
	if(InputSequenceConfig->InputActionsToUse == FGameplayTag::EmptyTag) return;

	FNavbarButtonData Data;
	Data.ButtonInputAction = UPDWGameSettings::GetInputActionByTag(InputSequenceConfig->InputActionsToUse);
	Data.bListenAction = true;
	Data.bListenAnyPlayer = false;

	MinigameNavButton->InitializeButton(Data);

	TriggerCallback = InTriggerCallBack;
	ProgressCallback = InProgressCallBack;
	
	MinigameNavButton->OnTriggerComplete.AddUniqueDynamic(this, &ThisClass::OnNavIconTrigger);
	MinigameNavButton->OnProgressUpdate.AddUniqueDynamic(this, &ThisClass::OnNavIconProgress);
	MinigameNavButton->SetVisibility(ESlateVisibility::HitTestInvisible);

	BP_StartMinigameInputActionIcon();
}

void UPDWSightWidget::RestoreSight()
{
	FoundTarget(false);
	BP_RestoreSight();
}

void UPDWSightWidget::InitSight(APDWPlayerController* OwnerWidget)
{
	SetOwningPlayer(OwnerWidget);
	if (MinigameNavButton)
	{
		MinigameNavButton->SetOwningPlayer(OwnerWidget);
	}
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this) && PlayerIndicator)
	{
		PlayerIndicator->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayerIndicator->InitIndicator(UPDWGameplayFunctionLibrary::IsSecondPlayer(OwnerWidget) ? EMiniGamePlayerType::P2 : EMiniGamePlayerType::P1);
	}
	else
	{
		PlayerIndicator->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPDWSightWidget::OnNavIconTrigger(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance)
{
	if (MinigameNavButton)
	{
		MinigameNavButton->OnTriggerComplete.RemoveDynamic(this, &ThisClass::OnNavIconTrigger);
		MinigameNavButton->OnProgressUpdate.RemoveDynamic(this, &ThisClass::OnNavIconProgress);
		MinigameNavButton->SetVisibility(ESlateVisibility::Hidden);
	}
	TriggerCallback.ExecuteIfBound(Instance);
}

void UPDWSightWidget::OnNavIconProgress(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance, float PreviousTime, float CurrentTime)
{
	ProgressCallback.ExecuteIfBound(Instance, PreviousTime, CurrentTime);
}