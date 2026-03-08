#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "FMODBlueprintStatics.h"

#include "NebulaGraphicsPlayLoopingSoundAnimNotifyState.generated.h"

class UFMODAudioComponent;
class UFMODEvent;

UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsPlayLoopingSoundAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	UFMODEvent* AudioEvent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool bAttachToMesh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true, EditCondition = "bAttachToMesh", EditConditionHides))
	bool bAutoDestroy = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FName SocketToAttach = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool bUseAudioParameter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true, EditCondition = "bUseAudioParameter", EditConditionHides))
	FName AudioParameter = NAME_None;

	// #DEV <Should be just 1 array, doing that because notify already in use and if i change we lose all the infos> [#daniele.m, 23 October 2025, ]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true, EditCondition = "bUseAudioParameter", EditConditionHides))
	TMap<FName,float> AudioParameters {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true, EditCondition = "bUseAudioParameter", EditConditionHides))
	float AudioParamterValue = 0.0f;

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	TMap<USkeletalMeshComponent*, UFMODAudioComponent*> CachedAudioComp = {};
	TMap<USkeletalMeshComponent*, FFMODEventInstance> CachedAudioInstance = {};
};