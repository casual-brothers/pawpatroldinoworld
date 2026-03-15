#pragma once

#include "Components/SceneComponent.h"
#include "Engine/DeveloperSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FMODStubTypes.generated.h"

namespace FMOD
{
namespace Studio
{
class EventInstance
{
public:
    EventInstance()
        : bValid(true)
        , bPaused(false)
    {
    }

    int stop(int)
    {
        bValid = false;
        return 0;
    }

    int setPaused(bool bInPaused)
    {
        bPaused = bInPaused;
        return 0;
    }

    int setParameterByName(const char*, float)
    {
        return 0;
    }

    int setVolume(float)
    {
        return 0;
    }

    int setPitch(float)
    {
        return 0;
    }

    bool isValid() const
    {
        return bValid;
    }

    void Play()
    {
        bValid = true;
        bPaused = false;
    }

private:
    bool bValid;
    bool bPaused;
};
}
}

enum
{
    FMOD_STUDIO_STOP_ALLOWFADEOUT = 0,
    FMOD_STUDIO_STOP_IMMEDIATE = 1
};

UENUM()
namespace EFMODEventProperty
{
    enum Type
    {
        ChannelPriority,
        ScheduleDelay,
        ScheduleLookahead,
        MinimumDistance,
        MaximumDistance,
        Count
    };
}

USTRUCT(BlueprintType)
struct FFMODEventInstance
{
    GENERATED_BODY()

    FFMODEventInstance()
        : Instance(nullptr)
    {
    }

    FMOD::Studio::EventInstance* Instance;
};

USTRUCT()
struct FFMODProjectLocale
{
    GENERATED_BODY()

    UPROPERTY(config, EditAnywhere, Category = Localization)
    FString LocaleName;

    UPROPERTY(config, EditAnywhere, Category = Localization)
    FString LocaleCode;

    UPROPERTY(config, EditAnywhere, Category = Localization)
    bool bDefault = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEventStopped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSoundStopped);

UCLASS(BlueprintType)
class FMODSTUDIO_API UFMODAsset : public UObject
{
    GENERATED_BODY()
};

UCLASS(BlueprintType)
class FMODSTUDIO_API UFMODEvent : public UFMODAsset
{
    GENERATED_BODY()
};

UCLASS(BlueprintType)
class FMODSTUDIO_API UFMODBank : public UFMODAsset
{
    GENERATED_BODY()
};

UCLASS(BlueprintType)
class FMODSTUDIO_API UFMODBus : public UFMODAsset
{
    GENERATED_BODY()
};

UCLASS(BlueprintType)
class FMODSTUDIO_API UFMODVCA : public UFMODAsset
{
    GENERATED_BODY()
};

UCLASS(config = Engine, defaultconfig)
class FMODSTUDIO_API UFMODSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, Category = Localization)
    TArray<FFMODProjectLocale> Locales;
};

UCLASS(Blueprintable, ClassGroup = (Audio, Common), meta = (BlueprintSpawnableComponent))
class FMODSTUDIO_API UFMODAudioComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UFMODAudioComponent();
    virtual ~UFMODAudioComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMODAudio)
    UFMODEvent* Event = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMODAudio)
    TMap<FName, float> ParameterCache;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMODAudio)
    FString ProgrammerSoundName;

    UPROPERTY(BlueprintAssignable)
    FOnEventStopped OnEventStopped;

    UPROPERTY(BlueprintAssignable)
    FOnSoundStopped OnSoundStopped;

    UPROPERTY()
    uint32 bAutoDestroy : 1;

    UPROPERTY()
    uint32 bStopWhenOwnerDestroyed : 1;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetEvent(UFMODEvent* NewEvent);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void Play();

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void Stop();

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void Release();

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    bool IsPlaying() const;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetPitch(float Pitch);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetPaused(bool bPaused);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    bool GetPaused() const;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetParameter(FName Name, float Value);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    float GetParameter(FName Name) const;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void GetParameterValue(FName Name, float& UserValue, float& FinalValue) const;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetProperty(EFMODEventProperty::Type Property, float Value);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    float GetProperty(EFMODEventProperty::Type Property) const;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    int32 GetLength() const;

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void SetProgrammerSoundName(FString Value);

    FMOD::Studio::EventInstance* StudioInstance;

private:
    bool bIsPlaying;
    bool bPaused;
};

UCLASS()
class FMODSTUDIO_API UFMODBlueprintStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static FFMODEventInstance PlayEvent2D(UObject* WorldContextObject, UFMODEvent* Event, bool bAutoPlay);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static FFMODEventInstance PlayEventAtLocation(UObject* WorldContextObject, UFMODEvent* Event, const FTransform& Location, bool bAutoPlay);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static UFMODAudioComponent* PlayEventAttached(UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName,
        FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay, bool bAutoDestroy);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void LoadBank(UFMODBank* Bank, bool bBlocking, bool bLoadSampleData);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void UnloadBank(UFMODBank* Bank);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static bool IsBankLoaded(UFMODBank* Bank);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void VCASetVolume(UFMODVCA* Vca, float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void SetGlobalParameterByName(FName Name, float Value);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static bool EventInstanceIsValid(FFMODEventInstance EventInstance);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void EventInstanceSetParameter(FFMODEventInstance EventInstance, FName Name, float Value);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void EventInstancePlay(FFMODEventInstance EventInstance);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void EventInstanceStop(FFMODEventInstance EventInstance, bool Release = false);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void EventInstanceRelease(FFMODEventInstance EventInstance);

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void SetLocale(const FString& Locale);
};
