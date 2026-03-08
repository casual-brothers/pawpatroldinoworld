// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/NebulaToolMovingActorsSpawner.h"
#include "GameplayTagContainer.h"
#include "PDWMovingNPCActorsSpawner.generated.h"

/**
 * 
 */
 class UPDWInteractionReceiverComponent;
 class UPDWDinoAnimInstance;

UCLASS()
class PDW_API APDWMovingNPCActorsSpawner : public ANebulaToolMovingActorsSpawner
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimationSetup")
	float MinRandomAnimationInterval = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimationSetup")
	float MaxRandomAnimationInterval = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimationSetup")
	float StopDistanceFromPlayers = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimationSetup")
	float StopDistanceFromVehicles = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimationSetup")
	float OptimizationDistance = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> RandomAnimations = {};

	UFUNCTION()
	void SetIsSplineActive(bool _IsActive) {IsActive = _IsActive; };

	UFUNCTION()
	bool GetIsSplineActive() { return IsActive; };

	UFUNCTION()
	void CheckPlayerDistance();

protected:

	void BeginPlay() override;

	UFUNCTION()
	void HandleRandomAnimation(float DeltaTime);

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);


	void Spawn() override;

private:

	UFUNCTION()
	void OnInteractionStart(const FPDWInteractionPayload& Payload);

	UFUNCTION()
	void OnInteractionEnd(UPDWInteractionBehaviour* Behaviour);

	UPROPERTY()
	AActor* ActorOnSpline = {};

	UPROPERTY()
	UPDWDinoAnimInstance* AnimInstanceRef = {};

	UPROPERTY()
	UPDWInteractionReceiverComponent* ReceiverCompRef = {};

	UPROPERTY()
	bool IsActive = true;

	UPROPERTY()
	float RandomAnimationInterval = 0.f;

	UPROPERTY()
	float RandomAnimationTimer = 0.f;

	UPROPERTY()
	bool DoingAnimation = false;

	UPROPERTY()
	bool DoingInteraction = false;

	UPROPERTY()
	bool IsPlayerClose = false;

	UPROPERTY()
	bool IsPlayerTooFar = false;

	void Tick(float DeltaTime) override;
};
