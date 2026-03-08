// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PDWSkillAnimationTriggerState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWSkillAnimationTriggerState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	EAnimationState AnimationStateToTrigger = EAnimationState::Skill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FGameplayTag PupMeshTag = FGameplayTag::EmptyTag;

private:

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
