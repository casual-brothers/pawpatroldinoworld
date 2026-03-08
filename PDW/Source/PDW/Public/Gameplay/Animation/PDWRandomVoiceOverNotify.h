// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Managers/PDWAudioManager.h"
#include "PDWRandomVoiceOverNotify.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWRandomVoiceOverNotify : public UAnimNotify
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FVoiceOverDialogueStruct> VoiceOverOptions;
private:
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	

};
