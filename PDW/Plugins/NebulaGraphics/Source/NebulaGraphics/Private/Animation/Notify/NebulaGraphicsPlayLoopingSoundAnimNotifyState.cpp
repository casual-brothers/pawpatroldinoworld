#include "Animation/Notify/NebulaGraphicsPlayLoopingSoundAnimNotifyState.h"

#include "FMODAudioComponent.h"

void UNebulaGraphicsPlayLoopingSoundAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!AudioEvent)
		return;

	if (bAttachToMesh)
	{
		UFMODAudioComponent* AudioComponent = UFMODBlueprintStatics::PlayEventAttached(AudioEvent, MeshComp, SocketToAttach, FVector::ZeroVector, EAttachLocation::SnapToTarget, true, true, true);
		if (bUseAudioParameter)
		{
			AudioComponent->SetParameter(AudioParameter, AudioParamterValue);
			for (const auto& [ParameterName, ParameterValue] : AudioParameters)
			{
				if (ParameterName.IsValid())
				{
					AudioComponent->SetParameter(ParameterName,ParameterValue);
				}
			}
			AudioComponent->bAutoDestroy = bAutoDestroy;
		}
		CachedAudioComp.Add(MeshComp, AudioComponent);
	}
	else
	{
		FFMODEventInstance EventInstance = UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, AudioEvent, SocketToAttach == NAME_None ? MeshComp->GetComponentTransform() : MeshComp->GetSocketTransform(SocketToAttach), true);
		if (bUseAudioParameter && AudioParameter != FName())
		{
			EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*AudioParameter.ToString()), AudioParamterValue);
			for (const auto& [ParameterName, ParameterValue] : AudioParameters)
			{
				if (ParameterName.IsValid())
				{
					EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*ParameterName.ToString()), ParameterValue);
				}
			}
		}
		CachedAudioInstance.Add(MeshComp, EventInstance);
	}

	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UNebulaGraphicsPlayLoopingSoundAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (bAttachToMesh)
	{
		if (CachedAudioComp.Contains(MeshComp) && IsValid(CachedAudioComp[MeshComp]))
		{
			CachedAudioComp[MeshComp]->Stop();
		}
	}
	else
	{
		if (CachedAudioInstance.Contains(MeshComp) && CachedAudioInstance[MeshComp].Instance)
		{
			CachedAudioInstance[MeshComp].Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		}
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}