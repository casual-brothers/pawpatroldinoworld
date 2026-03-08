#include "Animation/Notify/NebulaGraphicsPlaySoundAnimNotify.h"

#include "Components/SceneComponent.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"

void UNebulaGraphicsPlaySoundAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!AudioEvent)
		return;

	Super::Notify(MeshComp, Animation, EventReference);

	if (bCheckSurfaceMaterial)
	{
		AudioParamterValue = UNebulaFlowAudioFunctionLibrary::GetAudioSurfaceValue(MeshComp->GetOwner(), MeshComp->GetOwner());
	}

	if (MeshComp)
	{
		if (bAttachToMesh)
		{
			if (MeshComp->GetOwner())
			{
				UFMODAudioComponent* AudioComponent = UFMODBlueprintStatics::PlayEventAttached(AudioEvent, MeshComp, SocketToAttach, FVector::ZeroVector, EAttachLocation::SnapToTarget, false, true, true);
				if (AudioComponent)
				{
					if(bUseAudioParameter)
					{
						AudioComponent->SetParameter(AudioParameter, AudioParamterValue);
					}
					AudioComponent->bAutoDestroy = bAutoDestroy;
				}
			}
		}
		else
		{
			FFMODEventInstance EventInstance = UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, AudioEvent, SocketToAttach == NAME_None ? MeshComp->GetComponentTransform() : MeshComp->GetSocketTransform(SocketToAttach), true);
			if (bUseAudioParameter && AudioParameter != FName())
			{
				EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*AudioParameter.ToString()), AudioParamterValue);
			}
		}
	}
}