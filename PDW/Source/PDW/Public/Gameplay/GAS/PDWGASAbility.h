// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PDWGASAbility.generated.h"

class UPDWAbilityTask_OnTick;
class UPDWInteractionReceiverComponent;
class APDWPlayerState;
class APDWPlayerController;
class UPDWAnimInstance;

UENUM(BlueprintType,meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAbilityInputBehavior : uint8
{
	None =							0<<0,
	ShouldLoopOnInput =				1<<0,
	ShouldRestartOnInput =			1<<1,
	ShouldInterruptOnRelease =		1<<2,
	ShouldLoopOnHold =				1<<3,
};
ENUM_CLASS_FLAGS(EAbilityInputBehavior);

USTRUCT(BlueprintType)
struct PDW_API FMontageSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimMontage* MontageToPlay = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName SectionToLoop = FName("Loop");

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName EndingSection = FName("Success");

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName StartingSection = FName("Default");

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTag PupBackPackTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName NotifyName = FName("Trigger");

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName NotifyNameForInputCheck = FName("CheckInput");

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bEndSkillWithMontageEnd = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bActivateSkillState = true;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PDW_API UPDWGASAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPDWGASAbility(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	UPDWInteractionReceiverComponent* GetBestInteractionForTag(/*FGameplayTag FilterTag*/);
	
	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	/*const FGameplayTagContainer& GetSkillType();*/

protected:
	
	UFUNCTION(BlueprintNativeEvent, Category = Ability)
	void TriggerAbilityLogic();
	virtual void TriggerAbilityLogic_Implementation();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	virtual void OnReleaseInput(FGameplayEventData inPayLoad);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnReleaseInput();

	UFUNCTION()
	virtual void OnRestartAnimation(FGameplayEventData inPayLoad);
	UFUNCTION()
	virtual void OnLoopRequest(FGameplayEventData inPayLoad);
	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	virtual void OnNotify(FName NotifyName,const FBranchingPointNotifyPayload& BranchingPointPayload);

	virtual void OnCancelAbility();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCancelAbility();

	UFUNCTION(BlueprintCallable)
	UAnimInstance* GetAnimInstance();

	UFUNCTION(BlueprintCallable)
	virtual void OverrideMontageToPlaySettings();

	FOnMontageEnded MontageEndedDelegate;

	UFUNCTION()
	virtual void Tick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_Tick(float DeltaTime);

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/PDW.EAbilityInputBehavior"), Category = "Configuration")
	uint8 AbilityInputConfiguration;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FGameplayTagContainer SkillType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	FMontageSettings MontageSettings;

	UPROPERTY(EditDefaultsOnly)
	bool bTickAbility = false;

	UPROPERTY()
	TObjectPtr<UPDWAbilityTask_OnTick> TickTask{};

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APDWPlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APDWPlayerController> PlayerController;

	bool bHasBlueprintTickEnabled = false;

	bool bUseRear = false;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEventTask = nullptr;

	UPROPERTY()
	UAnimInstance* PupBackPackAnimInstance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* MeshComp = nullptr;

	bool bInputReleased = true;

	UPROPERTY()
	UPDWAnimInstance* PupAnimInstance = nullptr;

private:
	

	void SetUpInputBehavior(const uint8 inInputBehavior);
};
