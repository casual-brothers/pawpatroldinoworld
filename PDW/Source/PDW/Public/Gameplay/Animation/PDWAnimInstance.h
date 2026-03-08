// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "PDWAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	Walking = 0,
	Swimming = 1,
	Driving = 2,
	Flying = 3,
	Skill = 4,
	Sniff = 5,
};

UCLASS()
class PDW_API UPDWAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool EnabledOnCurrentPlatform();

	UFUNCTION(BlueprintCallable)
	void SetSpeedOverride(float NewValue) { SpeedOverride = NewValue; };

	UFUNCTION(BlueprintPure)
	float GetSpeedOverride() { return SpeedOverride; };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Interp)
	UAnimSequenceBase* AnimationToLoopForCutscene = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EEmotionType EmotionType = EEmotionType::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTalking = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag,TSoftObjectPtr<UAnimMontage>> AvailableMontages = {};

	UFUNCTION(BlueprintCallable)
	void SetAnimationState(const EAnimationState inNewState);

	UFUNCTION(BlueprintCallable)
	void SetDivingMode(const bool bInActive);

	UFUNCTION()
	const EAnimationState GetCurrentAnimationState() const;
	
	UFUNCTION(BlueprintGetter)
	bool GetSwimmingState() const;

	UFUNCTION(BlueprintGetter)
	bool GetDrivingState() const;

	UFUNCTION(BlueprintGetter)
	bool GetDivingState() const;

	UFUNCTION(BlueprintGetter)
	bool GetFlyingState() const;

	UFUNCTION(BlueprintGetter)
	bool GetSkillState() const;

	UFUNCTION(BlueprintGetter)
	bool GetSniffState() const;

	UFUNCTION(BlueprintGetter)
	bool GetCutsceneLoopAnimation() const;

	UFUNCTION(BlueprintGetter)
	bool GetIsInCutscene() const;

	UFUNCTION(BlueprintCallable)
	bool PlayMontageByTag(FGameplayTag MontageTag);

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UAnimMontage> GetMontageByTag(FGameplayTag MontageTag);

	void NativeBeginPlay() override;

	void BeginDestroy() override;

protected:

	EAnimationState CurrentAnimationState = EAnimationState::Walking;

	UPROPERTY(EditAnywhere,Category = "Notify Config")
	FPerPlatformBool bLogicEnabledOnCurrentPlatform;

	UPROPERTY(BlueprintReadOnly)
	bool bDiving = false;

	UPROPERTY()
	float SpeedOverride = -1.f;

	UPROPERTY()
	bool bIsInCutscene = false;

	UFUNCTION()
	void HandleCutsceneStart() { bIsInCutscene = true; };

	UFUNCTION()
	void HandleCutsceneEnd() { bIsInCutscene = false; };
};
