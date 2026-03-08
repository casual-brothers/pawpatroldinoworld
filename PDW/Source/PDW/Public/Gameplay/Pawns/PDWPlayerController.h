// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/NebulaFlowPlayerController.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "PDWPlayerController.generated.h"

class UPDWBreadCrumbsComponent;
class UGameplayHUD;
class APDWPlayerState;
class UPDWGASAbility;
class UPDWAttributeSet;
class APDWVehiclePawn;
class APDWCharacter;
class APDWAutoSwapArea;
class APDWExclusionCollisionVolume;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeCharacter, const FGameplayTag&, NewPup,APDWPlayerController*,inController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeCharacterRequested,APDWPlayerController*,inController);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwapVehicle, const bool, IsOnVehicle,APDWPlayerController*,inController );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPupCustomizationCompleted,APDWPlayerController*,InController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwapEnd,APDWPlayerController*,InController);


 UENUM(BlueprintType)
enum class EPupRequestType : uint8
{
	Next = 0,
	Current = 1,
};

UENUM(BlueprintType)
enum class ESkillSlotType : uint8
{
	Skill = 0,
	Interaction = 1,

};

/**
 *
 */
UCLASS()
class PDW_API APDWPlayerController : public ANebulaFlowPlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay()override;
	
	void InitPlayerAbilities();

	void AddMovementAbilities();

	UFUNCTION(BlueprintPure)
	UPDWBreadCrumbsComponent* GetBreadCrumbsComponent() { return BreadCrumbsComponent; }
	
	UFUNCTION(BlueprintPure)
	UPDWGASComponent* GetGASComponent() { return GASComponent; }
	
	UFUNCTION(BlueprintPure)
	APDWPlayerState* GetPDWPlayerState() { return PDWPlayerState; }

	UFUNCTION(BlueprintPure)
	UForceFeedbackComponent* GetForceFeedbackComponent() { return ForceFeedbackComponent; }

	UFUNCTION(BlueprintPure)
	UForceFeedbackEffect* GetVibrationsByTag(FGameplayTag VibrationTag);

	UPROPERTY(EditAnywhere,meta = (ForceInlineRow), Category = "Vibrations")
	TMap<FGameplayTag, UForceFeedbackEffect*> VibrationsConfig = {};

	void BeginDestroy() override;
	void SetNextPupForSwap(const FGameplayTag& inNewPup);
	const FGameplayTag& GetNextPupForSwap() const;
	
	UFUNCTION(BlueprintPure)
	const FGameplayTag GetCurrentPup()const;
	TSubclassOf<APawn> GetPupClassForSwap(const EPupRequestType& inPupRequest,bool bRequestVehicle = false) const;
	
	UFUNCTION(BlueprintCallable)
	APDWVehiclePawn* GetVehicleInstance();
	UFUNCTION(BlueprintPure)
	bool GetIsOnVehicle() const;

	void SetVehicleInstance(APDWVehiclePawn* inVehicle);
	UFUNCTION(BlueprintCallable)
	APDWCharacter* GetPupInstance() {return CharacterPup;}
	UFUNCTION(BlueprintCallable)
	void OnInteractFailed();
	UFUNCTION(BlueprintCallable)
	void ChangeCharacter(const FGameplayTag& inNewPup);
	UFUNCTION(BlueprintCallable)
	void ChangeVehicle(bool bMaintainVehicle = false,bool bUseCameraBlend = true);
	UPDWInteractionComponent* GetInteractionComponent();

	UFUNCTION(BlueprintCallable)
	void SetSprintDisabled(const bool bInDisable) { bDisableSprint = bInDisable; };
	UFUNCTION(BlueprintCallable)
	bool GetSprintDisabled() const {return bDisableSprint;};

	UFUNCTION(BlueprintCallable)
	FGameplayAbilitySpecHandle ActivateSkill(bool bPressed,TSubclassOf<UPDWGASAbility> inSkill = nullptr);
	UFUNCTION(BlueprintCallable)
	void MiniGameMove(const FVector2D& MovementVector,const FVector& inForwardVector,const FVector& inRightVector);

	void MiniGameEnd();

	UPROPERTY(BlueprintAssignable)
	FOnChangeCharacter OnChangeCharacter;

	UPROPERTY(BlueprintAssignable)
	FOnChangeCharacterRequested OnChangeCharacterRequested;

	UPROPERTY(BlueprintAssignable)
	FOnSwapVehicle OnSwapVehicle;

	UPROPERTY(BlueprintAssignable)
	FOnSwapVehicle OnSwapVehicleCompleted;

	UFUNCTION(BlueprintCallable)
	virtual void CancelSkillWithHandler(FGameplayAbilitySpecHandle& inHandler);

	UFUNCTION()
	void RefreshBreadcrumbsOwner();

	FOnPupCustomizationCompleted OnPupCustomizationCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnSwapEnd OnSwapEnd;

	UPROPERTY()
	bool bDoingSwappingAction = false;

	UPROPERTY()
	APDWAutoSwapArea* PendingSwapArea = {};

	UPROPERTY()
	APDWAutoSwapArea* CurrentSwapArea = {};

	UPROPERTY()
	APDWExclusionCollisionVolume* PendingExclusionVolume = {};

	UFUNCTION()
	void HandleSwapEnd();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DisableSkillEffect = nullptr;

	UPROPERTY()
	bool UsingMinigameCamera = false;

protected:

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void Interact();
	UFUNCTION(BlueprintCallable)
	void CharacterJump();
	UFUNCTION(BlueprintCallable)
	void Move(const FVector2D& Value);
	UFUNCTION(BlueprintCallable)
	void Look(const FVector2D& Value);
	UFUNCTION()
	void OnBeforeSave();

	UFUNCTION()
	void OnInteractionSuccess(const FPDWInteractionPayload& Payload);
	UFUNCTION()
	void OnInteractionStateAdded(UPDWInteractionReceiverComponent* Comp, const FGameplayTag& PrevTag, const FGameplayTag& NewTag);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDWBreadCrumbsComponent> BreadCrumbsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDWGASComponent> GASComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APDWPlayerState> PDWPlayerState;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UForceFeedbackComponent> ForceFeedbackComponent;

#pragma region BASICSKILLS
	UPROPERTY()
	TSubclassOf<UPDWGASAbility> SwapCharacterAbility{};
	UPROPERTY()
	TSubclassOf<UPDWGASAbility> SwapVehicleAbility{};
	UPROPERTY()
	TSubclassOf<UPDWGASAbility> InteractAbility{};
	UPROPERTY()
	TSubclassOf<UPDWGASAbility> JumpAbility{};
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWGASAbility> MovementAbilityClass{};
#pragma endregion


	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	const UPDWAttributeSet* BasicAttributeSet;

	UPROPERTY()
	TObjectPtr<APDWVehiclePawn> Vehicle;

	UPROPERTY()
	TObjectPtr<APDWCharacter> CharacterPup;

	FDelegateHandle BeforeSaveHandle;

	UPROPERTY()
	FGameplayAbilitySpecHandle CurrentSkillHandler = {};

private:

	void InitializeGASSystem();

	void SetUpAbilitiesOnPupSwap();

	FGameplayAbilitySpecHandle TestHandle;
	FGameplayAbilitySpec TestAbilitySpec;
	FGameplayTag NextPupForSwap = FGameplayTag::EmptyTag;
	bool bDisableSprint = false;
};
