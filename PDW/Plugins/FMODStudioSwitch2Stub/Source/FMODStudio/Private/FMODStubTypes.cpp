#include "FMODStubTypes.h"

#include "Engine/World.h"

namespace
{
TSet<TObjectPtr<UFMODBank>> GLoadedBanks;
FString GCurrentLocale;
}

UFMODAudioComponent::UFMODAudioComponent()
    : StudioInstance(nullptr)
    , bIsPlaying(false)
    , bPaused(false)
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoDestroy = false;
    bStopWhenOwnerDestroyed = false;
}

UFMODAudioComponent::~UFMODAudioComponent()
{
    Release();
}

void UFMODAudioComponent::SetEvent(UFMODEvent* NewEvent)
{
    Event = NewEvent;
}

void UFMODAudioComponent::Play()
{
    if (StudioInstance == nullptr)
    {
        StudioInstance = new FMOD::Studio::EventInstance();
    }

    StudioInstance->Play();
    bIsPlaying = true;
    bPaused = false;
}

void UFMODAudioComponent::Stop()
{
    if (StudioInstance != nullptr)
    {
        StudioInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    }

    bIsPlaying = false;
    bPaused = false;
    OnEventStopped.Broadcast();
    OnSoundStopped.Broadcast();

    if (bAutoDestroy)
    {
        DestroyComponent();
    }
}

void UFMODAudioComponent::Release()
{
    if (StudioInstance != nullptr)
    {
        delete StudioInstance;
        StudioInstance = nullptr;
    }

    bIsPlaying = false;
    bPaused = false;
}

bool UFMODAudioComponent::IsPlaying() const
{
    return bIsPlaying && StudioInstance != nullptr && StudioInstance->isValid();
}

void UFMODAudioComponent::SetVolume(float Volume)
{
    if (StudioInstance != nullptr)
    {
        StudioInstance->setVolume(Volume);
    }
}

void UFMODAudioComponent::SetPitch(float Pitch)
{
    if (StudioInstance != nullptr)
    {
        StudioInstance->setPitch(Pitch);
    }
}

void UFMODAudioComponent::SetPaused(bool bInPaused)
{
    bPaused = bInPaused;
    if (StudioInstance != nullptr)
    {
        StudioInstance->setPaused(bInPaused);
    }
}

bool UFMODAudioComponent::GetPaused() const
{
    return bPaused;
}

void UFMODAudioComponent::SetParameter(FName Name, float Value)
{
    ParameterCache.FindOrAdd(Name) = Value;
    if (StudioInstance != nullptr)
    {
        const FString ParameterName = Name.ToString();
        StudioInstance->setParameterByName(TCHAR_TO_UTF8(*ParameterName), Value);
    }
}

float UFMODAudioComponent::GetParameter(FName Name) const
{
    if (const float* Value = ParameterCache.Find(Name))
    {
        return *Value;
    }

    return 0.0f;
}

void UFMODAudioComponent::GetParameterValue(FName Name, float& UserValue, float& FinalValue) const
{
    UserValue = GetParameter(Name);
    FinalValue = UserValue;
}

void UFMODAudioComponent::SetProperty(EFMODEventProperty::Type, float)
{
}

float UFMODAudioComponent::GetProperty(EFMODEventProperty::Type) const
{
    return 0.0f;
}

int32 UFMODAudioComponent::GetLength() const
{
    return 0;
}

void UFMODAudioComponent::SetProgrammerSoundName(FString Value)
{
    ProgrammerSoundName = MoveTemp(Value);
}

static FFMODEventInstance MakeEventInstance(bool bAutoPlay)
{
    FFMODEventInstance EventInstance;
    EventInstance.Instance = new FMOD::Studio::EventInstance();
    if (bAutoPlay)
    {
        EventInstance.Instance->Play();
    }
    return EventInstance;
}

FFMODEventInstance UFMODBlueprintStatics::PlayEvent2D(UObject*, UFMODEvent*, bool bAutoPlay)
{
    return MakeEventInstance(bAutoPlay);
}

FFMODEventInstance UFMODBlueprintStatics::PlayEventAtLocation(UObject*, UFMODEvent*, const FTransform&, bool bAutoPlay)
{
    return MakeEventInstance(bAutoPlay);
}

UFMODAudioComponent* UFMODBlueprintStatics::PlayEventAttached(UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName,
    FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay, bool bAutoDestroy)
{
    UObject* Outer = AttachToComponent != nullptr ? static_cast<UObject*>(AttachToComponent) : GetTransientPackage();
    UFMODAudioComponent* AudioComponent = NewObject<UFMODAudioComponent>(Outer);
    if (AudioComponent == nullptr)
    {
        return nullptr;
    }

    AudioComponent->SetEvent(Event);
    AudioComponent->bStopWhenOwnerDestroyed = bStopWhenAttachedToDestroyed;
    AudioComponent->bAutoDestroy = bAutoDestroy;

    if (AttachToComponent != nullptr)
    {
        AudioComponent->SetupAttachment(AttachToComponent, AttachPointName);
        AudioComponent->SetRelativeLocation(Location);
        AudioComponent->RegisterComponent();
    }

    if (bAutoPlay)
    {
        AudioComponent->Play();
    }

    return AudioComponent;
}

void UFMODBlueprintStatics::LoadBank(UFMODBank* Bank, bool, bool)
{
    if (Bank != nullptr)
    {
        GLoadedBanks.Add(Bank);
    }
}

void UFMODBlueprintStatics::UnloadBank(UFMODBank* Bank)
{
    if (Bank != nullptr)
    {
        GLoadedBanks.Remove(Bank);
    }
}

bool UFMODBlueprintStatics::IsBankLoaded(UFMODBank* Bank)
{
    return Bank != nullptr && GLoadedBanks.Contains(Bank);
}

void UFMODBlueprintStatics::VCASetVolume(UFMODVCA*, float)
{
}

void UFMODBlueprintStatics::SetGlobalParameterByName(FName, float)
{
}

bool UFMODBlueprintStatics::EventInstanceIsValid(FFMODEventInstance EventInstance)
{
    return EventInstance.Instance != nullptr && EventInstance.Instance->isValid();
}

void UFMODBlueprintStatics::EventInstanceSetParameter(FFMODEventInstance EventInstance, FName Name, float Value)
{
    if (EventInstance.Instance != nullptr)
    {
        const FString ParameterName = Name.ToString();
        EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*ParameterName), Value);
    }
}

void UFMODBlueprintStatics::EventInstancePlay(FFMODEventInstance EventInstance)
{
    if (EventInstance.Instance != nullptr)
    {
        EventInstance.Instance->Play();
    }
}

void UFMODBlueprintStatics::EventInstanceStop(FFMODEventInstance EventInstance, bool ReleaseInstance)
{
    if (EventInstance.Instance != nullptr)
    {
        EventInstance.Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        if (ReleaseInstance)
        {
            delete EventInstance.Instance;
        }
    }
}

void UFMODBlueprintStatics::EventInstanceRelease(FFMODEventInstance EventInstance)
{
    if (EventInstance.Instance != nullptr)
    {
        delete EventInstance.Instance;
    }
}

void UFMODBlueprintStatics::SetLocale(const FString& Locale)
{
    GCurrentLocale = Locale;
}
