// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWPlaySequenceByInputBehaviour.h"
#include "LevelSequencePlayer.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"

void UPDWPlaySequenceByInputBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	const FSequenceConfiguration* SequenceConfig = InputActionsInfo[GetCurrentIndex()].GetPtr<FSequenceConfiguration>();
	ULevelSequence* SequenceToPlay = SequenceConfig->SequenceToPlay;
	ensureMsgf(SequenceToPlay,TEXT("PlaySequenceBehaviour - Missing Sequence to play"));
	if (SequencePlayer && LevelSequenceActor)
	{
		LevelSequenceActor->SetSequence(SequenceToPlay);
		SequencePlayer->Play();
		SequencePlayer->OnFinished.AddUniqueDynamic(this,&UPDWPlaySequenceByInputBehaviour::OnSequenceEnd);
	}
	BP_ExecuteBehaviour_Implementation(inInputInstance);
	/*Super::ExecuteBehaviour(inInputInstance);*/
}

void UPDWPlaySequenceByInputBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	Super::InitializeBehaviour(inController,inMiniGameComp);
	const FSequenceConfiguration* SequenceConfig = InputActionsInfo[GetCurrentIndex()].GetPtr<FSequenceConfiguration>();
	if(!SequenceConfig->SequenceToPlay)
		return;
	SequencePlayer = UPDWGameplayFunctionLibrary::PDWCreateLevelSequence(GetControllerOwner(),SequenceConfig->SequenceToPlay,SequenceConfig->LevelSequenceSettings,LevelSequenceActor);
	if (SequenceConfig->BindingTagID.IsValid())
	{
		LevelSequenceActor->SetBindingByTag(SequenceConfig->BindingTagID,{GetMiniGameOwner()->GetOwner()},false);
		if(SequenceConfig->bOverrideSequenceData)
		{
			UDefaultLevelSequenceInstanceData* Data = Cast<UDefaultLevelSequenceInstanceData>(LevelSequenceActor->DefaultInstanceData);
			if (Data)
			{
				Data->TransformOrigin = GetMiniGameOwner()->GetOwner()->GetActorTransform();
			}
		}

	}
	BindExitAndPauseAction();
}

void UPDWPlaySequenceByInputBehaviour::UninitializeBehaviour()
{
	BP_BeforeUninit();

	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.RemoveDynamic(this,&UPDWPlaySequenceByInputBehaviour::OnSequenceEnd);
		SequencePlayer->Stop();
	}

	Super::UninitializeBehaviour();
}

void UPDWPlaySequenceByInputBehaviour::OnSequenceEnd()
{
	const FSequenceConfiguration* SequenceConfig = InputActionsInfo[GetCurrentIndex()].GetPtr<FSequenceConfiguration>();
	const int32 InputInfoAmount = InputActionsInfo.Num();
	if (CurrentIndex < (InputInfoAmount - 1))
	{
		UpdateCurrentIndex();
		BindInputAction();
	}
	if (SequenceConfig->bNotifySuccess && GetControllerOwner())
	{
		FInteractionEventSignature InteractionEvent;
		InteractionEvent.EventTag = SequenceConfig->EventID;
		InteractionEvent.Interacter = GetControllerOwner();
		UPDWEventSubsytem::Get(GetControllerOwner())->TriggerInteractionEvent(SequenceConfig->TargetIDToNotify,InteractionEvent);
	}
}
