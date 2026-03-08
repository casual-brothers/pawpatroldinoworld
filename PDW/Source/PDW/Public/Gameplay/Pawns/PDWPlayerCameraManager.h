// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PDWPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API APDWPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	virtual void SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams) override;
protected:
	 void BeginPlay() override;

	 UFUNCTION()
	 void RemoveBlackScreen();

};
