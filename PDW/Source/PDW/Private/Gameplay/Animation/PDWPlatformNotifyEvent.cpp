// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Animation/PDWPlatformNotifyEvent.h"

bool UPDWPlatformNotifyEvent::IsLogicEnabledOnCurrentPlatform() const
{
	return LogicEnabledOnCurrentPlatform.GetDefault();
}

void UPDWPlatformNotifyEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if(!IsLogicEnabledOnCurrentPlatform())
		return;
	Super::Notify(MeshComp, Animation, EventReference);
}
