// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWAudioManager.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"

#if WITH_EDITOR
#include "ToxicUtilitiesSetting.h"
#endif

UPDWAudioManager* UPDWAudioManager::Get(UObject* WorldContext)
{
	return Cast<UPDWAudioManager>(UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(WorldContext));
}

void UPDWAudioManager::TriggerEnvironment(UObject* WorldContext, const FGameplayTag& inEnvironmentType)
{
	UPDWAudioManager* AudioMgr = UPDWAudioManager::Get(WorldContext);
	if (!AudioMgr->CurrentEnviromentinstance.EventInstance.Instance->isValid())
	{
		AudioMgr->PlayEnvironmentAudio();
	}
	if (!AudioMgr->CurrentMusicinstance.EventInstance.Instance->isValid())
	{
		AudioMgr->PlayMusicAudio();
	}
	UFMODBlueprintStatics::SetGlobalParameterByName(AudioMgr->GlobalEnvironmentParameterName,AudioMgr->EnvironmentParameterMap[inEnvironmentType]);
}

bool UPDWAudioManager::PlayVoiceOver(UObject* WorldContext, const FVoiceOverDialogueStruct& inVoiceOverConfig, TFunction<void()> CallBack /*= nullptr*/)
{
	if(inVoiceOverConfig.VoiceOverID.IsEmpty() || !inVoiceOverConfig.SpeakerID.IsValid())
		return false;

	UPDWAudioManager* AudioMgr = UPDWAudioManager::Get(WorldContext);
	if(!AudioMgr)
		return false;
	if (!AudioMgr->VoiceOverEventMap.Contains(inVoiceOverConfig.SpeakerID))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPDWAudioManager - Missing Audio Event for VO ID: %s"), *inVoiceOverConfig.SpeakerID.ToString());
		return false;
	}
	else
	{
		AudioMgr->CurrentVOID = inVoiceOverConfig.VoiceOverID;
		AudioMgr->PlayVoiceSpeaker(AudioMgr->VoiceOverEventMap[inVoiceOverConfig.SpeakerID],inVoiceOverConfig.VoiceOverID, CallBack);
		return true;
	}
}

void UPDWAudioManager::StopVoiceOver(UObject* WorldContext, FString VOToStop)
{
	UPDWAudioManager* AudioMgr = UPDWAudioManager::Get(WorldContext);
	if(!AudioMgr)
		return;

	if ((!VOToStop.IsEmpty()) && (VOToStop != AudioMgr->CurrentVOID))
	{
		return;
	}
	AudioMgr->StopVoiceSpeaker();
}

void UPDWAudioManager::ToggleMonoSnapShot(const bool inActive)
{
	if (inActive)
	{
		PlayStateAudioEvent(FName("MonoSnapShot"),MonoSnapShot);
	}
	else
	{
		StopStateAudioEvent(FName("MonoSnapShot"));
	}
}

UFMODEvent* UPDWAudioManager::GetSurfaceAudioValue(const EPhysicalSurface& inSurface, FName& OutParameterName, float& OutValue, float& OutImpactDelayValue)
{
	if (AudioSurfaceConfig.AudioEvent)
	{
		OutParameterName = AudioSurfaceConfig.SurfaceParameterName;
		OutImpactDelayValue = AudioSurfaceConfig.ImpactDelayTime;
		if (AudioSurfaceConfig.SurfacesMap.Contains(inSurface))
		{
			OutValue = AudioSurfaceConfig.SurfacesMap[inSurface];

			return AudioSurfaceConfig.AudioEvent;
		}
		else
		{
			OutValue = 0.f;
			return AudioSurfaceConfig.AudioEvent;
		}
	}
	return nullptr;
}

void UPDWAudioManager::PlayCutsceneMusicEvent(UFMODEvent* inEvent, const FName& ID)
{
	PlayStateAudioEvent("Cutscene",SnapshotMap["Cutscene"]);
	PlayStateAudioEvent(ID,inEvent);
}

void UPDWAudioManager::StopCutsceneMusicEvent(const FName& ID)
{
	StopStateAudioEvent("Cutscene");
	StopStateAudioEvent(ID,false);
}

void UPDWAudioManager::PlayMiniGameSoundtrack(const FGameplayTag& inMinigameTag)
{
	PlayStateAudioEvent("Minigame",SnapshotMap["Minigame"]);
	if (!MinigameSoundtrack.Contains(inMinigameTag))
	{
		return;
	}
	PlayStateAudioEvent("Cutscene",SnapshotMap["Cutscene"]);
	PlayStateAudioEvent(inMinigameTag.GetTagName(),MinigameSoundtrack[inMinigameTag]);
	//Snapshot to trigger
}

void UPDWAudioManager::StopMiniGameSoundtrack(const FGameplayTag& inMinigameTag)
{
	StopStateAudioEvent("Minigame");
	if (!MinigameSoundtrack.Contains(inMinigameTag))
	{
		return;
	}
	StopStateAudioEvent("Cutscene");
	StopStateAudioEvent(inMinigameTag.GetTagName(), false);
}

void UPDWAudioManager::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	Super::InitManager(InstanceOwner);
#if WITH_EDITOR
	if(UToxicUtilitiesSetting::Get()->bDisableAudio)
	{
		SetVolumeMaster(0);
	}
#endif
}

void UPDWAudioManager::PlayEnvironmentAudio()
{
	FFMODEventInstance NewInstance = UFMODBlueprintStatics::PlayEvent2D(this, EnvAudioEvent, true);
	FFModEventAndInstance NewEventAndinstance;
	NewEventAndinstance.Event = EnvAudioEvent;
	NewEventAndinstance.EventInstance = NewInstance;
	CurrentEnviromentinstance = NewEventAndinstance;
}

void UPDWAudioManager::PlayMusicAudio()
{
	FFMODEventInstance NewInstance = UFMODBlueprintStatics::PlayEvent2D(this, EnvAudioMusicEvent, true);
	FFModEventAndInstance NewEventAndinstance;
	NewEventAndinstance.Event = EnvAudioMusicEvent;
	NewEventAndinstance.EventInstance = NewInstance;
	CurrentMusicinstance = NewEventAndinstance;
}

void UPDWAudioManager::OnGameContextChanged(EGameContext NewContext)
{
	Super::OnGameContextChanged(NewContext);
	if (NewContext == EGameContext::EMainMenu)
	{
		PlayStateAudioEvent("MainMenu",GetRandomMusicByMap("MainMenu"));
	}
	else
	{
		StopStateAudioEvent("MainMenu",false);
	}
}
