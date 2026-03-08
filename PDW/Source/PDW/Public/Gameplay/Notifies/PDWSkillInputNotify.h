// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PDWSkillInputNotify.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PDW_API UPDWSkillInputNotify : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName LoopSection = NAME_None;

	UPROPERTY(EditAnywhere)
	FName NextSection = NAME_None;

	FString GetNotifyName_Implementation() const override;

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
