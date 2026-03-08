// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NebulaVehicle/NebulaVehicle.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"
#include "PDWVehiclePawn.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UWidgetComponent;
class UGameplayAbility;
class UOrbitSpringArmComponent;
class APDWPlayerController;
class UPDWGASAbility;
class UPDWGASComponent;
class UPDWAttributeSet;
class UPDWVehicleInteractionComponent;
class UPDWVehicleData;

DECLARE_LOG_CATEGORY_EXTERN(PDWVehicleLog, Log, All);

#define GAMEPLAYTAG_INAIR "State.InAir"
#define GAMEPLAYTAG_STUNT_BEFORE_LANDING_SIGNATURE "State.StuntBeforeLanding.Signature"
#define COLLISION_PROFILE_OVERLAP_PAWN "OverlapOnlyPawn"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupCollected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerfectStartComponentRegistered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyAttributeChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTagAdded, FGameplayTag, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTagRemoved, FGameplayTag, OldTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroundMaterialChanged, EPhysicalSurface, Surface);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCollision, float, SpeedKPH, EPhysicalSurface, Surface, FVector, HitLocation);

UENUM(BlueprintType)
enum class EVehicleType : uint8
{
	Vehicle,
	Truck,
	Special,
	WithSiren,
};

//# TODO ricorda di aggiungere l'interfaccia del player
UCLASS()
class APDWVehiclePawn : public ANebulaVehicle, public IPDWPlayerInterface,public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	APDWVehiclePawn();

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	EVehicleType VehicleType = EVehicleType::Vehicle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	float InPlaceSpeed = 5.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	float MinSteeringSpeed = 0.3f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	float AdditionalForwardInput = 0.2f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	float AfterSpawnAirTime = 1000.f;

	UFUNCTION(BlueprintCallable)
	void Hide(bool bHide);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Hide(bool bHIde);

	UFUNCTION(BlueprintCallable)
	void SpeedBoost(const float SpeedMultiplier,const float BoostTimer,const float AccellerationValue);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTurnDuringMinigame(int32 Direction);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponenthit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintPure)
	APDWPlayerController* GetPDWPlayerController() { return PDWPlayerController; }

#pragma region Setters

	void SetGravityMultiplier(float value) { GravityMultiplier = value; }

	void SetInitialVelocity(FVector inVelocity);

#pragma endregion

#pragma region Getters 

	UFUNCTION(BlueprintPure)
	bool GetBoosting() const {return bBoosting;};

	UFUNCTION(BlueprintPure)
	float GetPCTSpeed() { return MaxSpeed == 0 ? 0 : (GetSpeedKPH() / MaxSpeed); }

	UFUNCTION(BlueprintPure)
	float GetGroundDistance() { return GroundDistance; }

	UFUNCTION(BlueprintPure)
	float GetGravityMultiplier() { return GravityMultiplier; }

	UFUNCTION(BlueprintPure)
	bool GetSkipSwapGravity() { return SkipSwapGravity; }

	UFUNCTION(BlueprintPure)
	EPhysicalSurface GetCurrentGroundMaterialName() { return CurrentGroundMaterialName; }

	virtual float GetRPM() override {
		return abs(BodyLinearSpeed.X) * (60.f / (2.f * PI)) * RPMMultiplier;
	};

#pragma endregion

#pragma region Events

	UPROPERTY(BlueprintAssignable)
	FOnCollision OnCollision;

	UPROPERTY(BlueprintAssignable)
	FOnGroundMaterialChanged OnGroundMaterialChanged;

#pragma endregion

	void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

	UFUNCTION(BlueprintCallable)
	void AnalyzeInput(const FVector2D& MovementVector);

	void MiniGameMove(const FVector2D& MovementVector, const FVector& inForwardVector, const FVector& inRightVector) override;
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MiniGameMove(const FVector2D& MovementVector, const FVector& inForwardVector, const FVector& inRightVector);

	UPDWInteractionComponent* GetInteractionComponent() override;

	APDWPlayerState* GetPDWPlayerState() override;

	UFUNCTION(BlueprintCallable)
		void SetPhysActiveCarBP(bool inActive);

	UFUNCTION()
	void InitAttributes(const FGameplayTag& PupTag);


	void PhysicsGameTick(float DeltaTime) override;

	UFUNCTION()
	void CustomizeVehicle(FGameplayTag CurrentPup);

	UFUNCTION(BlueprintCallable)
	void ApplyVehicleDebuff(float SpeedMultiplier,bool bReset,float Timer = 0.0f,float AccelerationValue = 0);

	UFUNCTION(BlueprintCallable)
	void OverrideSuspensionsLenght(float NewSuspensionLenght);

	UFUNCTION(BlueprintCallable)
	void SetCustomizationSuspensionsLenght();

	UFUNCTION(BlueprintPure)
	float GetSuspensionsLenght();

	UFUNCTION(BlueprintCallable)
	void ResetSuspensionsLenght();

	/** Called for looking input */
	void Look(const FVector2D& LookAxisVector);

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	void InitCharacterAbilities();
	void OnSteeringChanged(const FOnAttributeChangeData& Data);
	void OnAccelerationChanged(const FOnAttributeChangeData& Data);
	void OnTopSpeedChanged(const FOnAttributeChangeData& Data);
	virtual void UpdatePhysicsCore(float DeltaTime) override;
	void UpdateAirStabilizer(float DeltaTime);
	void UpdateGravity(float DeltaTime);
	void CheckGround();

	virtual void UpdateSteering(float DeltaTime) override;

	UFUNCTION()
	void OnTakeOff();

	UFUNCTION()
	void OnLanding();

	float GroundDistance;
	EPhysicalSurface CurrentGroundMaterialName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Data")
	FGameplayTag PupTagTest;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Data")
	TObjectPtr<UPDWVehicleData> CurrentVehicleData = nullptr;

	bool bBoosting = false;
#pragma region Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	TObjectPtr<UPDWVehicleInteractionComponent> VehicleInteractionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VehicleMeshComponent{};

#pragma endregion

	float OriginalReverseSpeed = 0.0f;

#pragma region Air stabilizer

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|AirStabilizer", meta = (PDW))
	bool bUseAirStabilizer = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|AirStabilizer", meta = (PDW))
	TObjectPtr<UCurveFloat> PitchToAimOnVerticalSpeed{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|AirStabilizer", meta = (PDW))
	float AirStabilizerPitchBaseForce = 1.f;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|AirStabilizer")
	//	TObjectPtr<UCurveFloat> PitchForceMultiplierOverPitch {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|AirStabilizer")
	float AirStabilizerRollBaseForce = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|AirStabilizer")
	TObjectPtr<UCurveFloat> RollForceMultiplierOverRoll{};

#pragma endregion

#pragma region Gravity

	float GravityMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	bool bUseCustomGravity = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	float BaseGravity = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	float MaxNegativeVerticalSpeed = -30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	TObjectPtr<UCurveFloat> GravityOverTimeOnAir{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	float MinHeightForGravityOverTimeOnAir = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	float ResetGravityOverTimeOnAirDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	float GroundGravityStrenght = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	float MaxGroundSteepness = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Gravity")
	bool SkipSwapGravity = false;

#pragma endregion

#pragma region Coyote time

	bool bCoyoteTimeActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|CoyoteTime")
	bool bUseCoyoteTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|CoyoteTime")
	float CoyoteTimeDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|CoyoteTime")
	float CoyoteTimeSteering = 0.5f;

#pragma endregion

#pragma region Steering

	UPROPERTY()
	bool bInMiniGame = false;
	
	UPROPERTY()
	float MinigameSteeringMult = 0.f;

#pragma endregion

#pragma region Throttle

	UPROPERTY()
	float MinigameThrottleMult = 0.f;

#pragma  endregion

#pragma region Collision

	FVector TorqueToApply = FVector::ZeroVector;
	FVector ForceToApply = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Collision|Vehicles")
	float VehicleCollisionForceMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Collision|Vehicles")
	float VehicleCollisionTorqueMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Collision|Wall")
	float WallCollisionForceMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|Collision|Wall")
	float WallCollisionTorqueMultiplier = 1.f;

#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PDWVehicle|RPM")
	float RPMMultiplier = 10.f;

#pragma region Blueprint Implementable Events

#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDWGASComponent> GASComponent;

	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	const UPDWAttributeSet* BasicAttributeSet
	{
	};

	UPROPERTY()
	TObjectPtr<APDWPlayerController> PDWPlayerController;

	FVector InitialVelocity = FVector::ZeroVector;

	float InitialSuspensionLenght = 0.f;

	float AdditionalOnAirTimer = 0.f;

	bool WasInAir = false;

	float PreviousInAirTime = 0.f;

	float ResetGravityOverTimeOnAirTimer = 0.f;

private:

	bool bPhysicsEnabled = true;

	void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	FTimerHandle SpeedDebuffHandler;
};
