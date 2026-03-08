// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GAS/PDWSpawnerAbility.h"
#include "PDWFlyingAbility.generated.h"

class UPDWAnimInstance;

/**
 * 
 */
UCLASS()
class PDW_API UPDWFlyingAbility : public UPDWSpawnerAbility
{
	GENERATED_BODY()
	
protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void Tick(float DeltaTime) override;

	void FlyingLogic(const float DeltaTime);
	void CancelFlyght();
	void DescendLogic(const float DeltaTime);
	void OnTimerFinished();
	void OnReleaseInput(FGameplayEventData inPayLoad) override;

	void TransitionToExitAnimation();

	UFUNCTION()
	void OnPupSwap(APDWPlayerController* inController);

	UFUNCTION()
	void OnEnterAutoSwapArea(FGameplayEventData inPayLoad);

	UFUNCTION()
	void OnExitAutoSwapArea(FGameplayEventData inPayLoad);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bDebug=false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float DesiredFlyingAltitude = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float FallSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float FlyingAltitudeSmoothVelocity = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float FlyingDescendingSmoothVelocity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float FlyingDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FName StopFlySection = FName("Success");

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	
	bool bForceCancelAbility = false;
	bool bIsDescending = false;
	bool bTimerRunning = false;
	UPROPERTY()
	AActor* FlyingActor = nullptr;
	FTimerHandle FlyingTimerHandle;
	UPROPERTY()
	UPDWAnimInstance* AnimInstance = nullptr;

	bool bAutoRefresh = false;

public:
	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;


};
