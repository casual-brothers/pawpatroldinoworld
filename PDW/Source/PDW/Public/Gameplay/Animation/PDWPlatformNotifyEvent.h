// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PDWPlatformNotifyEvent.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWPlatformNotifyEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FPerPlatformBool LogicEnabledOnCurrentPlatform;

	bool IsLogicEnabledOnCurrentPlatform() const;

private:
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
