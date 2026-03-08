// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"
#include "Enums/NebulaGraphicsEnums.h"
#include "PDWCharacter.generated.h"

class UPDWSpringArmComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UPDWGASAbility;
class UPDWInteractionComponent;
class UPDWBreadCrumbsComponent;
class APDWPlayerController;
class UPDWAttributeSet;
class UPDWAnimInstance;
class UNebulaGraphicsCustomizationComponent;
class UCustomizationDataAsset;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputDelegate, FVector2D, NewValue);

UCLASS(config = Game)
class APDWCharacter : public ACharacter, public IGameplayTagAssetInterface, public IPDWPlayerInterface
{
	GENERATED_BODY()

public:
	APDWCharacter();
	/** Returns CameraBoom subobject **/
	FORCEINLINE class UPDWSpringArmComponent* GetCameraBoom() const { return AutomaticSpringArm; }

	void PreInitializeComponents() override;

	void InitCharacterAbilities();

	UFUNCTION(BlueprintCallable)
	void Hide(bool bHide,bool bSkipCollision = false);

	UFUNCTION(BlueprintPure)
	UPDWInteractionComponent* GetInteractionComponent() { return InteractionComponent; }
	UFUNCTION(BlueprintPure)
	UPDWGASComponent* GetGASComponent() { return GASComponent; }
	UFUNCTION(BlueprintPure)
	UPDWBreadCrumbsComponent* GetBreadCrumbsComponent();
	UFUNCTION(BlueprintPure)
	APDWPlayerController* GetPDWPlayerController() { return PDWPlayerController; }
	UFUNCTION(BlueprintPure)
	UPDWAnimInstance* GetAniminstance();

	float GetCachedGravityScale();

	UFUNCTION(BlueprintCallable)
	void EnableBouncingJumpForce(bool bEnable, float inValue = 0);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetForcedGravityScale(bool bEnabled, float Value);

	/** Called for movement input */
	void Move(const FVector2D& MovementVector);

	void MiniGameMove(const FVector2D& MovementVector,const FVector& inForwardVector,const FVector& inRightVector);

	/** Called for looking input */
	void Look(const FVector2D& LookAxisVector);

	UFUNCTION(BlueprintCallable)
	void NotifyJump();

	UPROPERTY(BlueprintReadOnly)
	bool StartJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool WaitForJumpNotify = false;

	UPROPERTY(BlueprintAssignable)
	FOnInputDelegate OnMovementInput;

	UPROPERTY(BlueprintAssignable)
	FOnInputDelegate OnCameraInput;

	APDWPlayerState* GetPDWPlayerState() override;

	void SetActivation(bool inActive) override;

	//TODO
	/*void ChangeOverlapEvents(bool IsActive) override;*/

	void BeginPlay() override;
	
	UFUNCTION()
	void CustomizePup(FGameplayTag PupTag);

	UFUNCTION()
	void SkeletalCustomizationComplete();
	
	void CollectOrCreateExistingDMIsFromMesh(UPrimitiveComponent* MeshComponent);
	void CollectDMIs();

	virtual void Landed(const FHitResult& Hit) override;
	void WaterLanded(const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeavyLandVelocity = 2000.f;
	
	UPROPERTY(Transient)
    // Container to hold all DMIs for easy access at runtime
    TArray<UMaterialInstanceDynamic*> AllDynamicMaterials;

	UPROPERTY(BlueprintReadWrite)
	bool bFlying = false;

	UFUNCTION(BlueprintPure)
	bool GetIsInsideGeyser() { return IsInsideGeyser; };

	UFUNCTION(BlueprintCallable)
	void SetIsInsideGeyser(bool _IsInsideGeyser) { IsInsideGeyser = _IsInsideGeyser; };

	UFUNCTION(BlueprintPure)
	float GetGeyserCurveValue() { return GeyserCurveValue; };

	UFUNCTION(BlueprintCallable)
	void SetGeyserCurveValue(float _GeyserCurveValue) { GeyserCurveValue = _GeyserCurveValue; };


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Customization Config")
	TMap<FGameplayTag,UCustomizationDataAsset*> Customizations;
protected:
	void OnSteeringChanged(const FOnAttributeChangeData& Data);
	void OnAccelerationChanged(const FOnAttributeChangeData& Data);
	void OnTopSpeedChanged(const FOnAttributeChangeData& Data);

	virtual void NotifyControllerChanged() override;

	virtual bool CanJumpInternal_Implementation() const;

	virtual void CheckJumpInput(float DeltaTime) override;

	virtual bool CheckJumpConditions();

	UFUNCTION()
	void DisableLogicWhileDriving(bool bInVehicleMode,APDWPlayerController* inController);

protected:



	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDWSpringArmComponent> AutomaticSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDWGASComponent> GASComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDWInteractionComponent> InteractionComponent;

	UPROPERTY()
	TObjectPtr<APDWPlayerController> PDWPlayerController;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	const UPDWAttributeSet* BasicAttributeSet
	{
	};


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump)
	float CoyoteTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump)
	float JumpBufferingTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump)
	TObjectPtr<UCurveFloat> AirTimeGravityScaleCurve{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump)
	float DefaultJumpValue = 0.f;

	float AirTime = 0.f;

	float JumpBufferingCount = 0.f;

	bool bForcedGravityActive = false;

	bool bDIsableLogicsWhileDriving = false;

	bool JumpNotifyArrived = false;

	bool CoyoteJumping = false;

	bool IsSwimming = false;

	bool bJumpBufferingActive = false;

	UPROPERTY()
	bool IsInsideGeyser = false;

	UPROPERTY()
	float GeyserCurveValue = 0.f;

private:
	void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	float CachedGravity = 0.0f;

	UPROPERTY()

	UNebulaGraphicsCustomizationComponent* NebulaGraphicsCustomizationComponent = {};

	//to think better
	int32 Count = 0;

public:

    // Function to change the FloatZ parameter on all materials
    UFUNCTION(BlueprintCallable, Category = "Materials")
    void SetFloatZParameter(float NewValue);

private:

	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	float WaterLandDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	float MaxWaterLandDistance = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	float MinWaterLandDistanceFromUnder = 80.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	float WaterLandVerticalSpeedToGetMaxDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	TEnumAsByte<ECollisionChannel> SwimmingChannel;
	
	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	TEnumAsByte<ECollisionChannel> WaterLandTrace;


	float WaterLand = 0.f;
	float CurrentWaterLandMultiplier= 0.f;

	UPROPERTY(EditDefaultsOnly, Category="WaterLanding")
	TObjectPtr<UCurveFloat> WaterLandCurve;
	
};

