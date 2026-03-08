// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PDWMovementSpeedNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMovementSpeedNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float NewSpeed = 0.f;

private:

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	float OriginalSpeed = 0.f;
};
