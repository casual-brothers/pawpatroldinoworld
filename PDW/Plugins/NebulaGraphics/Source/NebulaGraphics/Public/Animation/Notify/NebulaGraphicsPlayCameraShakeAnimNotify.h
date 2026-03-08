#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"

#include "NebulaGraphicsPlayCameraShakeAnimNotify.generated.h"

class UCameraShakeBase;

UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsPlayCameraShakeAnimNotify : public UAnimNotify
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TSubclassOf<UCameraShakeBase> CameraShake = nullptr;

private:

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};