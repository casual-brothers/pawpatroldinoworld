#include "Animation/Notify/NebulaGraphicsPlayCameraShakeAnimNotify.h"

#include "Kismet/GameplayStatics.h"

void UNebulaGraphicsPlayCameraShakeAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp->GetWorld() && CameraShake && MeshComp->GetOwner())
	{
		UGameplayStatics::PlayWorldCameraShake(MeshComp->GetWorld(), CameraShake, MeshComp->GetOwner()->GetActorLocation(), 0.0f, UE_BIG_NUMBER, 1.0f, true);
	}
}