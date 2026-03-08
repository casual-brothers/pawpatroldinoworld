#include "Managers/NebulaFlowAudioManager.h"
#include "FMODBlueprintStatics.h"
#include "Actors/Audio/NebulaFlowBaseAudioSpeaker.h"
#include "FMODAudioComponent.h"

const FName UNebulaFlowAudioManager::WeatherInstanceID			{ TEXT("WeatherEvent") };

void UNebulaFlowAudioManager::PlayStateAudioEvent(FName EventId, UFMODEvent* Event, bool AutoPlay)
{
	if (EventAndInstancesMap.Contains(EventId))
	{
		StopStateAudioEvent(EventId);
	}
	
	FFMODEventInstance NewInstance = UFMODBlueprintStatics::PlayEvent2D(this,Event,AutoPlay);
	FFModEventAndInstance NewEventAndinstance;
	NewEventAndinstance.Event = Event;
	NewEventAndinstance.EventInstance = NewInstance;
	EventAndInstancesMap.Add(EventId, NewEventAndinstance);
}

void UNebulaFlowAudioManager::StopStateAudioEvent(FName EventId, bool StopImmediate)
{
	if (EventAndInstancesMap.Contains(EventId))
	{
		EventAndInstancesMap[EventId].EventInstance.Instance->stop(StopImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT);
		EventAndInstancesMap.Remove(EventId);
	}
}

void UNebulaFlowAudioManager::SetStateAudioEventPaused(FName EventId, bool inPaused)
{
	if (EventAndInstancesMap.Contains(EventId))
	{
		EventAndInstancesMap[EventId].EventInstance.Instance->setPaused(inPaused);		
	}
}



void UNebulaFlowAudioManager::SetStateEventParameter(FName EventId, FName ParameterName, float Value)
{
	if (EventAndInstancesMap.Contains(EventId))
	{
		EventAndInstancesMap[EventId].EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*ParameterName.ToString()),Value);
	}
}

void UNebulaFlowAudioManager::PlayEnviromentByMapOrContext(bool bIgnoreMap)
{
	UFMODEvent* EnviromentEvent = GetEnviromentByMap(CurrentMap);
	if (!EnviromentEvent || bIgnoreMap)
	{
		EnviromentEvent = GetEnviromentByContext(CurrentContext);
	}
	if(EnviromentEvent)
	{ 
		StopEnviroment();
		FFMODEventInstance NewInstance = UFMODBlueprintStatics::PlayEvent2D(this, EnviromentEvent, true);
		FFModEventAndInstance NewEventAndinstance;
		NewEventAndinstance.Event = EnviromentEvent;
		NewEventAndinstance.EventInstance = NewInstance;
		CurrentEnviromentinstance = NewEventAndinstance;
		PlayWeatherEnviroment();
	}
}

void UNebulaFlowAudioManager::PlayMusicByMapOrContext(bool bIgnoreMap)
{
	UFMODEvent* MusicEvent = GetRandomMusicByMap(CurrentMap);
	if (!MusicEvent || bIgnoreMap)
	{
		MusicEvent = GetRandomMusicByContext(CurrentContext);
	}
	if (MusicEvent)
	{
		StopMusic();
		FFMODEventInstance NewInstance = UFMODBlueprintStatics::PlayEvent2D(this, MusicEvent, true);
		FFModEventAndInstance NewEventAndinstance;
		NewEventAndinstance.Event = MusicEvent;
		NewEventAndinstance.EventInstance = NewInstance;
		CurrentMusicinstance = NewEventAndinstance;
	}
}

void UNebulaFlowAudioManager::PlayWeatherEnviroment()
{
	if (bEnableWeatherAudioLogic)
	{
		if (WeatherAudioEvent)
		{
			PlayStateAudioEvent(WeatherInstanceID, WeatherAudioEvent);
		}
		else
		{
			UE_LOG(LogTemp, Warning,TEXT("Missing Weather Ambient additive Event!"));
		}
	}
}

void UNebulaFlowAudioManager::StopWeatherEnviroment()
{	
	StopStateAudioEvent(WeatherInstanceID);
}

void UNebulaFlowAudioManager::StopEnviroment()
{
	if (CurrentEnviromentinstance.EventInstance.Instance)
	{
		CurrentEnviromentinstance.EventInstance.Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
	StopWeatherEnviroment();
}
void UNebulaFlowAudioManager::StopMusic()
{
	if (CurrentMusicinstance.EventInstance.Instance)
	{
		CurrentMusicinstance.EventInstance.Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
}


void UNebulaFlowAudioManager::OnLoadLevelStarted(FName LevelName)
{
	Super::OnLoadLevelStarted(LevelName);
	CurrentMap = LevelName;
	if (IsValid(VoiceSpeaker))
	{
		VoiceSpeaker->Destroy();
		VoiceSpeaker = nullptr;
	}
}

void UNebulaFlowAudioManager::OnLoadLevelEnded(FName LevelName)
{
	Super::OnLoadLevelEnded(LevelName);
	PlayEnviromentByMapOrContext();
	if (bPlayRandomMusicOnContextChange)
	{
		PlayMusicByMapOrContext();
	}
}

void UNebulaFlowAudioManager::OnGameContextChanged(EGameContext NewContext)
{
	Super::OnGameContextChanged(NewContext);
	CurrentContext = NewContext;
	if (bPlayRandomEnviromentOnContextChange && NewContext == EGameContext::EGameplay)
	{
		PlayEnviromentByMapOrContext(true);
	}
	if (bPlayRandomMusicOnContextChange || NewContext == EGameContext::ELoading)
	{
		PlayMusicByMapOrContext(true);
	}
}


UFMODEvent* UNebulaFlowAudioManager::GetRandomMusicByContext(EGameContext Context)
{
	TArray<UFMODEvent*> _Events;
	for (FMusicAndEnviromentSoundTrackData& current : MusicSoundTracks)
	{
		if (current.Context == Context)
		{
			_Events.Add(current.MusicEvent);
		}
	}
	if (_Events.Num() > 0)
	{
		return _Events[FMath::RandRange(0, _Events.Num() - 1)];
	}
	return nullptr;
}

UFMODEvent* UNebulaFlowAudioManager::GetRandomMusicByMap(FName Map)
{
	TArray<UFMODEvent*> _Events;
	for (FMusicAndEnviromentSoundTrackData& current : MusicSoundTracks)
	{
		if (current.Map.IsEqual(Map))
		{
			_Events.Add(current.MusicEvent);
		}
	}
	if (_Events.Num() > 0)
	{
		return _Events[FMath::RandRange(0, _Events.Num() - 1)];
	}
	return nullptr;
}


UFMODEvent* UNebulaFlowAudioManager::GetEnviromentByContext(EGameContext Context)
{
	for (FMusicAndEnviromentSoundTrackData& current : EnviromentSoundTracks)
	{
		if (current.Context == Context)
		{
			return current.MusicEvent;
		}
	}
	return nullptr;
}

UFMODEvent* UNebulaFlowAudioManager::GetEnviromentByMap(FName Map)
{
	for (FMusicAndEnviromentSoundTrackData& current : EnviromentSoundTracks)
	{
		if (current.Map.IsEqual(Map))
		{
			return current.MusicEvent;
		}
	}
	return nullptr;
}

void UNebulaFlowAudioManager::OnVoiceSpeakerStopped()
{
	if (IsValid(VoiceSpeaker))
	{
		VoiceSpeaker->AudioComponent->OnEventStopped.RemoveDynamic(this,&UNebulaFlowAudioManager::OnVoiceSpeakerStopped);
		if(VoiceSpeakerCallback && VoiceSpeakerCallback!=nullptr)
		{ 
			VoiceSpeakerCallback();
		}
	}
}

UNebulaFlowAudioManager::UNebulaFlowAudioManager(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	bShouldTick = false;
}

void UNebulaFlowAudioManager::PlayVoiceSpeaker(UFMODEvent* inEvent, FString ProgrammerSound, TFunction<void()> CallBack)
{
	if (inEvent)
	{
		if (!IsValid(VoiceSpeaker))
		{
			VoiceSpeaker = GetWorld()->SpawnActor<ANebulaFlowBaseAudioSpeaker>();
			VoiceSpeaker->SetTickableWhenPaused(true);
		}
		if (IsValid(VoiceSpeaker))
		{
			VoiceSpeaker->PlayAudioEvent(inEvent,ProgrammerSound);
			VoiceSpeakerCallback = CallBack;
			VoiceSpeaker->AudioComponent->OnEventStopped.AddUniqueDynamic(this,&UNebulaFlowAudioManager::OnVoiceSpeakerStopped);
		}
	}
}

void UNebulaFlowAudioManager::ExecuteAudioOperation(FFsmAudioOperation& AudioOperation)
{
	//TO DO -> FIX WITH ENVIROMENT TYPE
	switch (AudioOperation.OperationType)
	{
		case (EFsmAudioOperationType::ENone):
		{
			return;
		}
		break;
		case(EFsmAudioOperationType::EPlayRandomMusic):
		{
			PlayMusicByMapOrContext();
		}
		break;
		case(EFsmAudioOperationType::EStopRandomMusic):
		{
			StopMusic();
		}
		break;
		case(EFsmAudioOperationType::EPlayEnviroment):
		{
			PlayEnviromentByMapOrContext();
		}
		break;
		case(EFsmAudioOperationType::EStopEnviroment):
		{
			StopEnviroment();
		}
		break;
		case(EFsmAudioOperationType::EPlay):
		{
			PlayStateAudioEvent(AudioOperation.EventId,AudioOperation.FModEvent,AudioOperation.bAllowAutoPlay);
		}
		break;
		case(EFsmAudioOperationType::EStop):
		{
			StopStateAudioEvent(AudioOperation.EventId,!AudioOperation.bAllowFadeout);
		}
		break;
		case(EFsmAudioOperationType::EPause):
		{
			SetStateAudioEventPaused(AudioOperation.EventId,true);
		}
		break;
		case(EFsmAudioOperationType::EUnpause):
		{
			SetStateAudioEventPaused(AudioOperation.EventId,false);
		}
		break;
		case(EFsmAudioOperationType::ESetParameter):
		{
			SetStateEventParameter(AudioOperation.EventId,AudioOperation.ParameterName,AudioOperation.ParameterValue);
		}
		break;
	}

}

void UNebulaFlowAudioManager::SetMusicVolume(float Volume)
{
	for (int i = 0; i < FMODBGMVCA.Num(); i++)
	{
		UFMODBlueprintStatics::VCASetVolume(FMODBGMVCA[i], Volume);
		//UFMODBlueprintStatics::BusSetVolume(FMODVoicesBus[i], Volume);
	}
}

void UNebulaFlowAudioManager::SetVolumeSfx(float Volume)
{
	for (int i = 0; i < FMODSFXVCA.Num(); i++)
	{
		UFMODBlueprintStatics::VCASetVolume(FMODSFXVCA[i], Volume);

		//UFMODBlueprintStatics::BusSetVolume(FMODVoicesBus[i], Volume);
	}
}

void UNebulaFlowAudioManager::SetVolumeVoices(float Volume)
{
	for (int i = 0; i < FMODVoicesVCA.Num(); i++)
	{
		UFMODBlueprintStatics::VCASetVolume(FMODVoicesVCA[i], Volume);


		//UFMODBlueprintStatics::BusSetVolume(FMODVoicesBus[i], Volume);
	}
}

void UNebulaFlowAudioManager::SetVolumeMaster(float Volume)
{
	UFMODBlueprintStatics::VCASetVolume(FMODMasterVCA,Volume);
}

void UNebulaFlowAudioManager::StopVoiceSpeaker()
{
	if (IsValid(VoiceSpeaker))
	{
		VoiceSpeakerCallback=nullptr;
		VoiceSpeaker->StopAudio();
		VoiceSpeaker->Destroy();
		VoiceSpeaker = nullptr;
	}
}

void UNebulaFlowAudioManager::UpdateAudioDayNightCycle(const float InNormalizedDayTime)
{
	if (bEnableDayTimeAudioLogic && DayNightAudioParameterName != NAME_None)
	{
		SetEnviromentAudioParameter(DayNightAudioParameterName, InNormalizedDayTime, true);
		if (bDebugDayTime)
		{
			GEngine->AddOnScreenDebugMessage(987324, 0.1f, FColor::Green, FString::Printf(TEXT("Current Day Time %f"), InNormalizedDayTime));
		}
	}
}

void UNebulaFlowAudioManager::UpdateAudioWindIntensity(const float InWindIntensity)
{
	if (bEnableWeatherAudioLogic && WindIntensityAudioParameterName != NAME_None)
	{
		SetEnviromentAudioParameter(WindIntensityAudioParameterName, InWindIntensity, true);
		if (bDebugWeather)
		{
			GEngine->AddOnScreenDebugMessage(987314, 0.1f, FColor::Orange, FString::Printf(TEXT("Current Wind Intensity %f"), InWindIntensity));
		}
	}
}

void UNebulaFlowAudioManager::UpdateAudioRainIntensity(const float InRainIntensity)
{
	if (bEnableWeatherAudioLogic && RainAudioParameterName != NAME_None)
	{
		SetEnviromentAudioParameter(RainAudioParameterName, InRainIntensity, true);
		if (bDebugWeather)
		{
			GEngine->AddOnScreenDebugMessage(987314, 0.1f, FColor::Blue, FString::Printf(TEXT("Current Precipitation Intensity %f"), InRainIntensity));
		}
	}
}

void UNebulaFlowAudioManager::SetEnviromentAudioParameter(const FName& ParameterName, float Value, bool bGlobalParameter /*=false*/)
{
	if (CurrentEnviromentinstance.EventInstance.Instance)
	{
		if (!bGlobalParameter)
		{
			CurrentEnviromentinstance.EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*ParameterName.ToString()), Value);
		}
		else
		{
			UFMODBlueprintStatics::SetGlobalParameterByName(ParameterName, Value);
		}
	}
}
