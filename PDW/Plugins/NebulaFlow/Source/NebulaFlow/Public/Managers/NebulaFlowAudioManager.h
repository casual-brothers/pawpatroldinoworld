
#pragma once


#include "NebulaFlowManagerBase.h"
#include "FMODBlueprintStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Actors/Audio/NebulaFlowBaseAudioSpeaker.h"
#include "Templates/Function.h"

#include "NebulaFlowAudioManager.generated.h"

class UFMODBank;
class UFMODBus;
class UFMODEvent;
class UFMODVCA;
class ANebulaflowBaseAudioSpeaker;
struct FTableRowBase;


UENUM(BlueprintType)
enum class EFsmAudioOperationType : uint8
{
	ENone,
	EPlay,
	EPlayRandomMusic,
	EStopRandomMusic,
	EPlayEnviroment,
	EStopEnviroment,
	EStop,
	EPause,
	EUnpause,
	ESetParameter
};

USTRUCT(BlueprintType)
struct FMusicAndEnviromentSoundTrackData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Audio")
	UFMODEvent* MusicEvent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
	EGameContext Context = EGameContext::EIntro;

	UPROPERTY(EditAnywhere, Category = "Audio")
	FName Map =NAME_None;

};

USTRUCT(BlueprintType)
struct FFsmAudioOperation
{
	
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Audio")
	EFsmAudioOperationType OperationType = EFsmAudioOperationType::ENone;

	UPROPERTY(EditAnywhere, Category = "Audio")
	FName EventId = NAME_None;
	
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (EditCondition = "OperationType == EFsmAudioOperationType::EPlay"))
	UFMODEvent* FModEvent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio", meta = (EditCondition = "OperationType == EFsmAudioOperationType::EPlay"))
	bool bAllowAutoPlay = true; 

	UPROPERTY(EditAnywhere, Category = "Audio", meta = (EditCondition = "OperationType == EFsmAudioOperationType::EStop"))
	bool bAllowFadeout = false;

	UPROPERTY(EditAnywhere, Category = "Audio", meta = (EditCondition = "OperationType == EFsmAudioOperationType::ESetParameter"))
	FName ParameterName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Audio", meta = (EditCondition = "OperationType == EFsmAudioOperationType::ESetParameter"))
	float ParameterValue = 0.f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	bool bOnExitState = false;

};

USTRUCT()
struct FFModEventAndInstance
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
	UFMODEvent* Event = nullptr;

	UPROPERTY()
	FFMODEventInstance EventInstance{};

};

USTRUCT(BlueprintType)
struct NEBULAFLOW_API FAudioSurfaceTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> Surface = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttenuationCoefficient = 0.0f;

};


UCLASS(Blueprintable)
class NEBULAFLOW_API UNebulaFlowAudioManager : public UNebulaFlowManagerBase
{

	GENERATED_BODY()

public:

	UNebulaFlowAudioManager(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODBus*> FMODSFXBus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODBus*> FMODBGMBus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODBus*> FMODVoicesBus;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODVCA*> FMODSFXVCA;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODVCA*> FMODBGMVCA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODVCA*> FMODVoicesVCA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		UFMODVCA* FMODMasterVCA = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODBank*> FMODDefaultBanks;//To be loaded at start and remain forever
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODBank*> FMODMenuVOBanks;//to be loaded before the menu and change when change language
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD")
		TArray<UFMODBank*> FMODZone;//to be loaded when enter in the particular zone and removed when back to menu

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD|Enviroment")
	bool bPlayRandomEnviromentOnContextChange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD|Music")
	bool bPlayRandomMusicOnContextChange = false;

	void PlayVoiceSpeaker(UFMODEvent* inEvent, FString ProgrammerSound = FString(""),TFunction<void()> CallBack = nullptr);

	void ExecuteAudioOperation(FFsmAudioOperation& AudioOperation);

	void SetMusicVolume(float Volume);

	void SetVolumeSfx(float Volume);

	void SetVolumeVoices(float Volume);

	void SetVolumeMaster(float Volume);

	void StopVoiceSpeaker();

	FFModEventAndInstance GetCurrentMusicinstance() { return CurrentMusicinstance; };

	virtual void UpdateAudioDayNightCycle(const float InNormalizedDayTime);

	virtual void UpdateAudioWindIntensity(const float InWindIntensity);

	virtual void UpdateAudioRainIntensity(const float InRainIntensity);

	static const FName WeatherInstanceID;
	
	UFUNCTION()
	FString GetCurrentAudioLanguage() { return CurrentLanguage; };

	UFUNCTION()
	void SetCurrentAudioLanguage(FString NewLang) { CurrentLanguage = NewLang; };

protected:
	
	UPROPERTY(EditAnywhere, Category = "DEBUG")
	bool bDebugDayTime = false;
	
	UPROPERTY(EditAnywhere, Category = "DEBUG")
	bool bDebugWeather = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD|Config")
	bool bEnableDayTimeAudioLogic = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|FMOD|Config")
	bool bEnableWeatherAudioLogic = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bEnableDayTimeAudioLogic", EditConditionHides), Category = "Audio|FMOD|Config|DayTime")
	FName DayNightAudioParameterName = FName("day_night");	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bEnableWeatherAudioLogic", EditConditionHides), Category = "Audio|FMOD|Config|Weather")
	FName WindIntensityAudioParameterName = FName("wind");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bEnableWeatherAudioLogic", EditConditionHides), Category = "Audio|FMOD|Config|Weather")
	FName RainAudioParameterName = FName("rain");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bEnableWeatherAudioLogic", EditConditionHides), Category = "Audio|FMOD|Config|Weather")
	TObjectPtr<UFMODEvent> WeatherAudioEvent = nullptr;

	UPROPERTY()
	TMap<FName,FFModEventAndInstance> EventAndInstancesMap{};

	UPROPERTY()
	FFModEventAndInstance CurrentEnviromentinstance{};

	UPROPERTY()
	FFModEventAndInstance CurrentMusicinstance {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (TitleProperty = "Map"), Category = "Audio|FMOD|Enviroment")
	TArray<FMusicAndEnviromentSoundTrackData> EnviromentSoundTracks{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (TitleProperty = "Map"), Category = "Audio|FMOD|Music")
	TArray<FMusicAndEnviromentSoundTrackData> MusicSoundTracks{};

	void PlayStateAudioEvent(FName EventId, UFMODEvent* Event, bool AutoPlay = true);

	void StopStateAudioEvent(FName EventId, bool StopImmediate = true);

	void SetStateAudioEventPaused(FName EventId, bool inPaused);

	void SetStateEventParameter(FName EventId, FName ParameterName, float Value);

	virtual void SetEnviromentAudioParameter(const FName& ParameterName, float Value, bool bGlobalParameter =false);

	void StopMusic();
	void StopEnviroment();

	virtual void OnLoadLevelStarted(FName LevelName) override;
	virtual void OnLoadLevelEnded(FName LevelName) override;
	virtual void OnGameContextChanged(EGameContext NewContext) override;
	virtual void PlayEnviromentByMapOrContext(bool bIgnoreMap = false);
	virtual void PlayMusicByMapOrContext(bool bIgnoreMap = false);
	virtual void PlayWeatherEnviroment();
	virtual void StopWeatherEnviroment();

	FName CurrentMap = NAME_None;

	EGameContext CurrentContext;

	UFMODEvent* GetRandomMusicByContext(EGameContext Context);
	UFMODEvent* GetRandomMusicByMap(FName Map);
	UFMODEvent* GetEnviromentByContext(EGameContext Context);
	UFMODEvent* GetEnviromentByMap(FName Map);
private:
	

	//VoiceSpeaker

	UPROPERTY()
	ANebulaFlowBaseAudioSpeaker* VoiceSpeaker = nullptr;

	UFUNCTION() 
	void OnVoiceSpeakerStopped();

	TFunction<void()> VoiceSpeakerCallback =nullptr;

	UPROPERTY()
		FString CurrentLanguage = FString("");};