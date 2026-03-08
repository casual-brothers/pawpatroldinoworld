// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Managers/NebulaFlowAudioManager.h"
#include "GameplayTagContainer.h"
#include "PDWAudioManager.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FVoiceOverDialogueStruct
{
	GENERATED_USTRUCT_BODY()
public:

	//this is the key associated to the programmer name of the voice over
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(Editcondition = "bCustomizeVoiceOverID",EditConditionHides), Category = "DESIGN")
	FString VoiceOverID = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
	FGameplayTag SpeakerID = FGameplayTag::EmptyTag;
};

// #DEV <if needed change param value and name in a tmap of a tmap to make it iper generic> [#daniele.m, 16 October 2025, ]

USTRUCT(BlueprintType)
struct FSimpleAudioStruct
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFMODEvent* AudioEvent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ParameterName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ParameterValue = 0.0f;

};

USTRUCT(BlueprintType)
struct FAudioSurfaceStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFMODEvent* AudioEvent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SurfaceParameterName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta = (ForceInlineRow))
	TMap<TEnumAsByte<EPhysicalSurface>,float> SurfacesMap = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ImpactDelayTime = 0.5f;
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWAudioManager : public UNebulaFlowAudioManager
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static UPDWAudioManager* Get(UObject* WorldContext);

	static void TriggerEnvironment(UObject* WorldContext,const FGameplayTag& inEnvironmentType);

	static bool PlayVoiceOver(UObject* WorldContext,const FVoiceOverDialogueStruct& inVoiceOverConfig, TFunction<void()> CallBack = nullptr);
	static void StopVoiceOver(UObject* WorldContext, FString VOToStop = "");

	void ToggleMonoSnapShot(const bool inActive);
	UFMODEvent* GetSurfaceAudioValue(const EPhysicalSurface& inSurface,FName& OutParameterName,float& OutValue, float& OutImpactDelayValue);

	void PlayCutsceneMusicEvent(UFMODEvent* inEvent, const FName& ID);
	void StopCutsceneMusicEvent(const FName& ID);

	void PlayMiniGameSoundtrack(const FGameplayTag& inMinigameTag);
	void StopMiniGameSoundtrack(const FGameplayTag& inMinigameTag);

	void InitManager(UNebulaFlowGameInstance* InstanceOwner) override;

protected:
	
	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	TMap<FGameplayTag, UFMODEvent*> MinigameSoundtrack;

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	TMap<FName,UFMODEvent*> SnapshotMap;

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	FAudioSurfaceStruct AudioSurfaceConfig;

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	FName GlobalEnvironmentParameterName = FName("Env");

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	UFMODEvent* EnvAudioEvent = nullptr;

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	UFMODEvent* EnvAudioMusicEvent = nullptr;

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig")
	UFMODEvent* MonoSnapShot = nullptr;

	UPROPERTY(EditAnywhere, Category = "PDW | AudioConfig",meta = (ForceInlineRow))
	TMap<FGameplayTag,float> EnvironmentParameterMap;

	UPROPERTY(EditAnywhere, Category = "PDW | VoiceOverConfig",meta = (ForceInlineRow))
	TMap<FGameplayTag, UFMODEvent*> VoiceOverEventMap;

	void PlayEnvironmentAudio();
	void PlayMusicAudio();


	void OnGameContextChanged(EGameContext NewContext) override;

private:

	UPROPERTY()
	FString CurrentVOID = "";
};
