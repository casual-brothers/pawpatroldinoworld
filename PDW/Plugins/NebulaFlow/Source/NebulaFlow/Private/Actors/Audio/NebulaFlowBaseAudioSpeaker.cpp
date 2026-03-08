#include "Actors/Audio/NebulaFlowBaseAudioSpeaker.h"
#include "FMODAudioComponent.h"

ANebulaFlowBaseAudioSpeaker::ANebulaFlowBaseAudioSpeaker(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	
	AudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(FName("FMODAudioComponent"));
	AudioComponent->SetTickableWhenPaused(true);
	if (AudioComponent)
	{
		AudioComponent->SetupAttachment(GetRootComponent());
		AudioComponent->SetAutoActivate(false);
	}

}

ANebulaFlowBaseAudioSpeaker::~ANebulaFlowBaseAudioSpeaker()
{
	if (IsValid(AudioComponent))
	{
		AudioComponent->Release();
	}
}

void ANebulaFlowBaseAudioSpeaker::PlayAudioEvent(UFMODEvent* inEvent, FString ProgrammerSoundName)
{
	if (AudioComponent)
	{
		if (AudioComponent->IsPlaying())
		{
			AudioComponent->Stop();
		}
		AudioComponent->SetEvent(inEvent);
		if (!ProgrammerSoundName.IsEmpty())
		{
			AudioComponent->SetProgrammerSoundName(ProgrammerSoundName);
		}
		AudioComponent->Play();
	}

}

bool ANebulaFlowBaseAudioSpeaker::IsPlayingAudio()
{
	return AudioComponent && AudioComponent->IsPlaying();
}

void ANebulaFlowBaseAudioSpeaker::StopAudio()
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
	}
}