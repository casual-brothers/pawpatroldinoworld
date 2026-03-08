#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"

#include "Engine/DataTable.h"
#include "UI/NebulaFlowUIConstants.h"
#include "UI/NebulaFlowWidgetDataStructures.h"
#include "FMODBlueprintStatics.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowAudioManager.h"
#include "Components/NebulaGameplaySurfaceControlComponent.h"
#include "Settings/NebulaFlowAudioSettings.h"


FFMODEventInstance UNebulaFlowAudioFunctionLibrary::PlayUIEvent(UObject* WorldContextObject, FName EventId)
{
	UNebulaFlowUIConstants * UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(WorldContextObject);
	if(UIConstants)
	{ 
		const UDataTable* AudioTable = UIConstants->GetUIAudioTable();
		if (AudioTable)
		{
			if (EventId != FName(""))
			{
				FAudioEventsTableRow* Row = AudioTable->FindRow<FAudioEventsTableRow>(EventId, FString("AUDIO"));
				if (Row && Row->AudioEvent)
				{
					if (!Row->ProgrammerName.IsEmpty())
					{
						UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(WorldContextObject);
						if (AudioManager)
						{
							AudioManager->PlayVoiceSpeaker(Row->AudioEvent, Row->ProgrammerName);
						}
					}
					else if (!Row->ParameterName.IsEmpty())
					{
						FFMODEventInstance EventInstance = UFMODBlueprintStatics::PlayEvent2D(WorldContextObject, Row->AudioEvent, true);
						if (EventInstance.Instance)
						{
							EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*FName(Row->ParameterName).ToString()), Row->ParameterValue);
							if (!Row->ParameterNameB.IsEmpty())
							{
								EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*FName(Row->ParameterNameB).ToString()), Row->ParameterValueB);
							}
						}						
					}
					else
					{
						return UFMODBlueprintStatics::PlayEvent2D(WorldContextObject, Row->AudioEvent, true);
					}
				}
			}
		}
	}
	//returning Empty Instance
	FFMODEventInstance Instance;
	Instance.Instance = nullptr;
	return Instance;
}

FFMODEventInstance UNebulaFlowAudioFunctionLibrary::PlayEventAtLocation(UObject* WorldContextObject, UFMODEvent* inEvent, FVector Location)
{
	return UFMODBlueprintStatics::PlayEventAtLocation(WorldContextObject,inEvent,FTransform(FRotator::ZeroRotator,Location,FVector::OneVector),true);
}

void UNebulaFlowAudioFunctionLibrary::PlayVoiceSpeaker(UObject* WorldContextObject, UFMODEvent* AudioEvent, FString ProgrammerSoundName /*= FString("")*/,TFunction<void()> InCallback)
{
	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(WorldContextObject);
	if (AudioManager)
	{
		AudioManager->PlayVoiceSpeaker(AudioEvent,ProgrammerSoundName,InCallback);
	}
}

void UNebulaFlowAudioFunctionLibrary::ForceStopSpeaker(UObject* WorldContextObject)
{
	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(WorldContextObject);
	if (AudioManager)
	{
		AudioManager->StopVoiceSpeaker();
	}
}

FFMODEventInstance UNebulaFlowAudioFunctionLibrary::PlaySnapShot(UObject * WorldContextObject, FName EventID)
{
	UNebulaFlowUIConstants * UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(WorldContextObject);
	if (UIConstants)
	{
		const UDataTable* AudioTable = UIConstants->GetUIAudioTable();
		if (AudioTable)
		{
			FAudioEventsTableRow* Row = AudioTable->FindRow<FAudioEventsTableRow>(EventID, FString("AUDIO"));
			if (Row && Row->AudioEvent)
			{
				return UFMODBlueprintStatics::PlayEvent2D(WorldContextObject, Row->AudioEvent, true);
			}
		}
	}
	//returning Empty Instance
	FFMODEventInstance Instance;
	Instance.Instance = nullptr;
	return Instance;
}



void UNebulaFlowAudioFunctionLibrary::StopSnapShot(FFMODEventInstance Instance)
{
	if (Instance.Instance)
	{
		Instance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
}

float UNebulaFlowAudioFunctionLibrary::GetAudioSurfaceValue(UObject * WorldContextObject, const AActor* inActor)
{
	UNebulaGameplaySurfaceControlComponent* SurfaceControlComponent = inActor->FindComponentByClass<UNebulaGameplaySurfaceControlComponent>();
	if (SurfaceControlComponent)
	{
		return UNebulaFlowAudioSettings::GetSoundSurfaceValue(SurfaceControlComponent->GetCurrentGroundSurface());
	}
	return 0.0f;
}