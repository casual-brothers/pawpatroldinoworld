// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "PDWSplineRunPlayerTwoBehaviour.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class PDW_API UPDWSplineRunPlayerTwoBehaviour : public UPDWMovingObjectBehaviour
{
	GENERATED_BODY()
	

public:
	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;

	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;

	void CustomTick(const float inDeltaTime) override;

	UPROPERTY(EditAnywhere,Category = "Config")
	float LerpSpeed = 100.0f;

	UPROPERTY(EditAnywhere,Category = "Config")
	float SideDistance = 300.0f;

	UPROPERTY(EditAnywhere,Category = "Config")
	UNiagaraSystem* SpawnVFX = nullptr;
protected:

	void Move(int32 NewIndex);

private:

	bool bIsTransitioning = false;
	FVector TargetRelativeLocation;
	int32 CurrentSplineIndex = 0;
	FVector CenterLocation;
	

	TWeakObjectPtr<USkeletalMeshComponent> FakeChase;
};
