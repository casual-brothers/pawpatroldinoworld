#pragma once



#include "GameFramework/Actor.h"
#include "NebulaFlowBaseAudioSpeaker.generated.h"

class UFMODAudioComponent;
class UNebulaFlowAudioManager;

UCLASS()
class NEBULAFLOW_API ANebulaFlowBaseAudioSpeaker : public AActor
{
	GENERATED_BODY()

	friend class UNebulaFlowAudioManager;

public:

	ANebulaFlowBaseAudioSpeaker(const FObjectInitializer& ObjectInitializer);

	~ANebulaFlowBaseAudioSpeaker();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	bool IsPlayingAudio();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Audio")
	UFMODAudioComponent* AudioComponent = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	virtual void PlayAudioEvent(UFMODEvent* inEvent,FString ProgrammerSoundName = FString(""));

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopAudio();

};