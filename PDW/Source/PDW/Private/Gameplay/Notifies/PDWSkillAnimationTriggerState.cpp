// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Notifies/PDWSkillAnimationTriggerState.h"
#include "Gameplay/Animation/PDWAnimInstance.h"

void UPDWSkillAnimationTriggerState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if(!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	USkeletalMeshComponent* TargetMeshComp = Owner->FindComponentByTag<USkeletalMeshComponent>(PupMeshTag.GetTagName());
	if (!TargetMeshComp)
		return;
	if (UPDWAnimInstance* AnimInstance = Cast<UPDWAnimInstance>(TargetMeshComp->GetAnimInstance()))
	{
		if(AnimInstance->GetDrivingState())
			return;
		AnimInstance->SetAnimationState(AnimationStateToTrigger);
	}
}

void UPDWSkillAnimationTriggerState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	USkeletalMeshComponent* TargetMeshComp = Owner->FindComponentByTag<USkeletalMeshComponent>(PupMeshTag.GetTagName());
	if (!TargetMeshComp)
		return;
	if (UPDWAnimInstance* AnimInstance =	Cast<UPDWAnimInstance>(TargetMeshComp->GetAnimInstance()))
	{
		if(AnimInstance->GetDrivingState())
			return;
		AnimInstance->SetAnimationState(EAnimationState::Walking);
	}
}
