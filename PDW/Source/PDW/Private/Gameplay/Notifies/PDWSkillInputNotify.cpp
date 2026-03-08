// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Notifies/PDWSkillInputNotify.h"


void UPDWSkillInputNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	MeshComp->GetAnimInstance()->Montage_SetNextSection(LoopSection,NextSection);
}

FString UPDWSkillInputNotify::GetNotifyName_Implementation() const
{
	if (LoopSection.IsValid())
	{
		return LoopSection.ToString();
	}
	return UAnimNotify::GetNotifyName_Implementation();
}
	