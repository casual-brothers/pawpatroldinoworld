// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Flow/States/PDWVideoFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "BinkMediaPlayer.h"
#include "BinkMediaTexture.h"


#include "MediaPlayer.h"
#include "FileMediaSource.h"

void UPDWVideoFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	//UPDWAudioManager* AudioManager = UPDWAudioManager::Get(GetWorld());
	//if (AudioManager)
	//{
	//	AudioManager->SetMasterVolume(1.f);
	//	AudioManager->SetMusicVolume(1.f);
	//	AudioManager->SetVolumeSfx(1.f);
	//	AudioManager->SetVolumeVoices(1.f);
	//}
#if WITH_EDITOR
	if (bSkipVideos)
	{
		TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
	}
	else
#endif
	{
		CurrentVideoIndex = -1;
		StartNextVideo();
	}
}


void UPDWVideoFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (CurrentVideoIndex < Videos.Num() && Videos[CurrentVideoIndex].bSkippable)
	{
		OnVideoEnded();
	}
}

void UPDWVideoFSMState::StartNextVideo()
{
	CurrentVideoIndex++;

	if (Videos.Num() > CurrentVideoIndex)
	{
		FPDWVideoAndAudio& CurrentVideo = Videos[CurrentVideoIndex];
		
		//MediaPlayer->OnMediaOpened.AddUniqueDynamic(this, &UPPRVideoFSMState::OnVideoStarted);
		
#if PLATFORM_SWITCH
		auto SwitchVideo = CurrentVideo.SwitchVideoToPlay.LoadSynchronous();
		if(SwitchVideo)
		{
			MediaPlayer->OnEndReached.AddUniqueDynamic(this, &UPDWVideoFSMState::OnVideoEnded);
			MediaPlayer->OpenSource(SwitchVideo);

			if (Instance.Instance)
			{
				UFMODBlueprintStatics::EventInstanceStop(Instance);
			}
			if (CurrentVideo.VideoSound != nullptr)
			{
				Instance = UFMODBlueprintStatics::PlayEvent2D(this, CurrentVideo.VideoSound, true);
			}
		}
#else
		UBinkMediaPlayer* Video = CurrentVideo.BinkVideo.LoadSynchronous();
		if (Video)
		{
			BinkMediaTexture->SetMediaPlayer(Video);
			Video->OnMediaReachedEnd.AddUniqueDynamic(this, &UPDWVideoFSMState::OnVideoEnded);
			Video->InitializePlayer();
			Video->Play();
			if (Instance.Instance)
			{
				UFMODBlueprintStatics::EventInstanceStop(Instance);
			}
			if (CurrentVideo.VideoSound != nullptr)
			{
				Instance = UFMODBlueprintStatics::PlayEvent2D(this, CurrentVideo.VideoSound, true);
			}
		}		
#endif
		else
		{
			StartNextVideo();
		}
	}
	else
	{
		if (Videos.Num() > 0 && Videos[0].BinkVideo)
		{
			BinkMediaTexture->SetMediaPlayer(nullptr);
		}
		TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
	}
}

void UPDWVideoFSMState::OnVideoEnded()
{
	if (BinkMediaTexture)
	{
		FPDWVideoAndAudio& CurrentVideo = Videos[CurrentVideoIndex];
#if PLATFORM_SWITCH
		MediaPlayer->OnEndReached.RemoveAll(this);
#else
		UBinkMediaPlayer* Video = CurrentVideo.BinkVideo.LoadSynchronous();
		Video->OnMediaReachedEnd.RemoveAll(this);
		Video->Stop();
#endif
	}
	if (Instance.Instance)
	{
		UFMODBlueprintStatics::EventInstanceStop(Instance);
	}
	StartNextVideo();
}
