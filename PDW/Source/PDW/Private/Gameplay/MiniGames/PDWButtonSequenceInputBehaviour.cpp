// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWButtonSequenceInputBehaviour.h"
#include "EnhancedInputComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Data/PDWGameSettings.h"
#include "Managers/PDWHUDSubsystem.h"

void UPDWButtonSequenceInputBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	UpdateCurrentIndex();
	BindInputAction();
	Super::ExecuteBehaviour(inInputInstance);
}

void UPDWButtonSequenceInputBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	if (SequenceBehaviour == ESequenceBehaviour::Random)
	{
		ShuffleSequence();
	}
	Super::InitializeBehaviour(inController, inMiniGameComp);	
}

void UPDWButtonSequenceInputBehaviour::UninitializeBehaviour()
{
	//reset
	CurrentIndex = 0;
	Super::UninitializeBehaviour();
}

void UPDWButtonSequenceInputBehaviour::ShuffleSequence()
{
	int32 LastIndex = InputActionsInfo.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		const int32 RandomIndex = FMath::RandRange(0, i);
		InputActionsInfo.Swap(i, RandomIndex);
	}
}

void UPDWButtonSequenceInputBehaviour::BindInputAction()
{
	if (CurrentIndex > InputActionsInfo.Num() -1)
	{
		return;
	}
			
	FNavButtonTriggerCallback TriggerCallback;
	TriggerCallback.BindLambda([&, this](const FInputActionInstance& inInputInstance)
	{
		ExecuteBehaviour(inInputInstance);
		});

	FNavButtonProgressCallback ProgressCallback;
	ProgressCallback.BindLambda([&, this](const FInputActionInstance& inInputInstance, float PreviousTime, float CurrentTime)
		{
			OnProgressUpdate(inInputInstance, PreviousTime, CurrentTime);
		});
	if (!GetControllerOwner())
		return;
	UPDWHUDSubsystem::Get(this)->RequestInputActionIcon(GetControllerOwner(), InputActionsInfo[CurrentIndex], TriggerCallback, ProgressCallback);
}

const int32 UPDWButtonSequenceInputBehaviour::GetCurrentIndex() const
{
	return CurrentIndex;
}

void UPDWButtonSequenceInputBehaviour::UpdateCurrentIndex()
{
	CurrentIndex++;
}