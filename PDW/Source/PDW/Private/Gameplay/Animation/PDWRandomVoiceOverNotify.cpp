// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Animation/PDWRandomVoiceOverNotify.h"

void UPDWRandomVoiceOverNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (VoiceOverOptions.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, VoiceOverOptions.Num() - 1);
		FVoiceOverDialogueStruct SelectedVoiceOver = VoiceOverOptions[RandomIndex];
		if (MeshComp && MeshComp->GetWorld())
		{
			UPDWAudioManager::PlayVoiceOver(MeshComp->GetWorld(), SelectedVoiceOver);
		}
	}
}
