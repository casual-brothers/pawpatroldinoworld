// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWMinigameNavButton.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"

void UPDWMinigameNavButton::InitializeButton(FNavbarButtonData NewButtonData)
{
	bAlreadyTriggered = false;
	Super::InitializeButton(NewButtonData);
	BP_InitButton();
}

void UPDWMinigameNavButton::OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (bAlreadyTriggered)
	{
		return;
	}
	bAlreadyTriggered = true;
	Super::OnInputTriggered(Instance, inPC);
	TriggerActionInstance = Instance;
	if (AudioInstance.Instance)
	{
		AudioInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
}

void UPDWMinigameNavButton::CompleteMinigameButton()
{
	UninitializeButton();
	OnTriggerComplete.Broadcast(this, TriggerActionInstance);
	if(bSelfRemove) RemoveFromParent();
}

void UPDWMinigameNavButton::OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (bIsOneShot && bAlreadyTriggered)
	{
		return;
	}
	float PreviousTime = CurrentTime;
	Super::OnInputGoing(Instance, inPC);
	if (PreviousTime != CurrentTime)
	{
		OnProgressUpdate.Broadcast(this, Instance, PreviousTime, CurrentTime);
	}

	if (!AudioInstance.Instance)
	{
		AudioInstance = UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), ProgressSoundName);
	}
	AudioInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*ProgressSoundParameterName.ToString()), CurrentTime);
}
