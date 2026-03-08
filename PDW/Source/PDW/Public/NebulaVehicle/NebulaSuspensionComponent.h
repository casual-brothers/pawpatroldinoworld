#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CollisionShape.h"
#include "Engine/World.h"

//5.4
#include "Engine/HitResult.h"
#include "NebulaVehicle/NebulaVehicle.h"
#include "NebulaSuspensionComponent.generated.h"

class UCurveFloat;
class UCurveTable;

UCLASS(meta=(BlueprintSpawnableComponent))
class PDW_API UNebulaSuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNebulaSuspensionComponent();
	
	virtual void UpdateTick(float DeltaTime);
	virtual void UpdatePhysics(float DeltaTime);
	virtual void UpdateSteeringGeometry(float DeltaTime);
	virtual void UpdateLateralForce(float DeltaTime);
	// To override in child class
	virtual void UpdateLateralForceCut(float DeltaTime) {}

#pragma region Settors
	
	UFUNCTION(BlueprintCallable, Category = "NebulaSuspensionComponent")
		void SetStiffness(float inStifness);

	UFUNCTION(BlueprintCallable, Category = "NebulaSuspensionComponent")
		void SetDamping(float inDamping);

	UFUNCTION(BlueprintCallable, Category = "NebulaSuspensionComponent")
		void SetExtensionLength(float inLength);

	UFUNCTION(BlueprintCallable, Category = "NebulaSuspensionComponent")
		void SetWheelScale(float inScale);

	UFUNCTION(BlueprintCallable, Category = "NebulaSuspensionComponent")
		void RecalculateWheelRadius();

	void SetSteeringInput(float inSteering, float sensitivity);

	void SetHandbrakeInput(bool bInHandbrakeOn);
	
	void SetBrakeInput(float input) { BrakeInput = input; }

	void SetVehicleOwner(ANebulaVehicle* inVehicle) { VehicleOwner = inVehicle; VehicleBodyInstance = VehicleOwner->GetBodyInstance(); }

	void SetSteerOnAir(bool bInSteerOnAir) { bSteerOnAir = bInSteerOnAir; }

#pragma endregion

#pragma region Gettors
	
	//Returns the static mesh component, not to be confused with a pure static mesh. If the wheel's current static mesh is required, use 'GetStaticMesh' on this return.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		UStaticMeshComponent* GetWheelMeshComponent();

	//Returns if the suspension component, or more specifically the wheel, is in contact with the surface.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		bool IsInAir();

	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		bool IsTooSteep();

	//Returns the current length of the suspension. This is the difference between the maximum extension length and the compression distance due to the total weight.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		float GetCurrentLength();

	//Returns the hit result data from the raycast.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		FHitResult GetHitResult();

	//Returns how much compression the spring is undergoing. This is a normalized value between 0 and 1.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		float GetCompression();

	//Returnw whether the wheel affects the handbrake;
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		bool IsHandbrakeWheel();

	//Returns the steering rotation amount in degrees.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		float GetSteeringRotation();

	//Returns the relative offset in the Z axis used to locate the wheel.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		float GetWheelRelativeOffset();

	//Returns the wheel local rotation used for animating the wheel. Units are Radians per second.
	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		FRotator GetWheelLocalRotation();

	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		float GetExtensionLenght() {return ExtensionLength; };

	UFUNCTION(BlueprintPure, Category = "NebulaSuspensionComponent")
		bool IsSteeringWheel();

	FVector GetWheelLinearVelocity() { return LinearVelocity; }

	float GetCurrentLateralForce() { return CurrentLateralForce; }

	float GetCurrentLateralForceCut() { return CurrentLateralForceCut; }

	float GetGroundDistance() { return GroundDistance; }

#pragma endregion

#pragma region Server

	UPROPERTY(ReplicatedUsing=OnRep_Steering)
		float ServerSteeringRotation;

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdates;
	float ClientStartSteeringRotation;

	UFUNCTION()
		void OnRep_Steering();

#pragma endregion

protected:

	virtual void BeginPlay() override;

	ANebulaVehicle* VehicleOwner {};
	FBodyInstance* VehicleBodyInstance {};
	
#pragma region Core

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Core")
		bool bRenderCustomDepthPass = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Core")
		bool bIsRearWheel = false;

	// Whether or not this wheel will have steering input applied to.
	UPROPERTY(EditAnywhere, Category = "Wheel|Core")
		bool bIsSteeringWheel = false;

	// This is a purely visual variable. If the wheel is facing in the wrong direction (ussually the left set of wheels), set this to true.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|Core")
		bool bRotateWheel = false;

	//Whether the wheel will be affected by the handbrake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Core")
		bool bHandbrakeWheel = false;

#pragma endregion

#pragma region Suspension

	FCollisionQueryParams QueryParam {};

	// Stiffness of the suspension in Newtons per meter N/m. A higher stiffness will result in a higher resting length and springier behaviour.
	// This can be thought of as the resistance to a deforming force, such as weight.
	// It should be noted that the heavier the mass, the higher the stiffness needed to support that weight.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Suspension")
		float Stiffness = 22500.f;

	// This is a dimensionless coefficient and affects the rate at which the suspension spring reaches static equilibrium.
	// A higher damping value will result in less bouncy behaviour.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Suspension")
		float Damping = 1250.f;

	// This is the maximum length that the spring will extend to from it's starting location. Units are in centimeters (cm).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Suspension")
		float ExtensionLength = 50.f;

	UPROPERTY(EditAnywhere, Category = "Wheel|Suspension")
		TEnumAsByte<ECollisionChannel>  CollisionChannel = ECollisionChannel::ECC_WorldStatic;

#pragma endregion

#pragma region Mesh

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|Mesh")
		UStaticMesh* WheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|Mesh")
		TArray<UMaterialInterface*> WheelMaterials;

#pragma endregion

#pragma region Lateral force

	float CurrentLateralForce = 0.f;
	bool bSteerOnAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|LateralForce")
		float LateralForceScalar = 2000.f;

	// This is the maximum force that a tyre will produce in Newtons (N).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|LateralForce")
		float MaxLateralForce = 8500.f;

	// This is the distance offset of the force application point to the center of mass. Changing this value affects how much the vehicle will rolls. 
	// Negative values are also valid and may even be necessary to correct for the desired roll amount.
	// It can be seen as somewhat of an anti-roll bar scaling factor.
	UPROPERTY(EditAnywhere, Category = "Wheel|LateralForce")
		float ContactForceOffsetLocation = 0.f;

#pragma endregion

#pragma region Custom rotation

	//This adds rotation to the wheel as a constant. This is useful for burnout at zero speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|CustomRotation")
		float BurnoutRotation = 0.f;

	// This is simply a scalar for wheel velocity. E.g during a burnout or drifting this can be increased in blueprints to reflect that state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|CustomRotation")
		float WheelAngularVelocityMultiplier = 1.f;

#pragma endregion

#pragma region Steering

	// This is the maximum angle of wheels set to steer. This affets the lateral force direction, and consequently the vehicle behaviour.
	// Lateral force is applied perpendicular to the forward direction of the wheel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel|Steering", meta = (EditCondition = "bIsSteeringWheel==true", EditConditionHides))
		float MaxSteeringAngle = 40.f;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension|InputResponse")
		float SteeringSpeed = 2.f;

	// This is how quickly the steer wheel rotates to zero degrees. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|Steering", meta = (EditCondition = "bIsSteeringWheel==true", EditConditionHides))
		float SteeringReleaseSpeed = 12.f;

#pragma endregion

#pragma region Raycast parameter

	// This uses spheretrace and switches to a line raycast when the spheretrace becomes unsuitable. 
	// If this is set to false, a pure line raycast will be used which may be more suitable for arcade behaviour 
	UPROPERTY(EditAnywhere, Category = "Wheel|Raycast")
		bool bHybridRaycast = true;

	//The radius used for the sphere trace. The radius will automatically be set by the wheelMesh. If no mesh is specified then this value will be used. This value is in meters (m).
	UPROPERTY(EditAnywhere, Category = "Wheel|Raycast")
		float Radius = 33.f;

	// This is one of the conditions for switching to a line raycast. This is the normalized impact normal threshold, values closer to 1 are of a higher tolerance which will result in the trace being predominantly a sphere trace.
	UPROPERTY(EditAnywhere, Category = "Wheel|Raycast", meta = (EditCondition = "bHybridRaycast==true", EditConditionHides), meta = (ClampMin = "0"), meta = (ClampMax = "1"))
		float ImpactNormalTolerance = 0.9f;

	// In centimeters (cm). This is the second of the conditions for switching to a line raycast. This is the maximum height of the wheel that is valid for a sphere trace collision.
	// Lower this value to limit the sphere contact region closer to the base of the wheel.
	UPROPERTY(EditAnywhere, Category = "Wheel|Raycast", meta = (EditCondition = "bHybridRaycast==true", EditConditionHides))
		float ImpactHeightTolerance = 30.f;

#pragma endregion

#pragma region IdleLock

	// Whether or not to lock to wheels animation when completely stationary to prevent micro rotations.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|IdleLock")
		bool bUseIdleLock = true;

	// How long after stationary to lock the wheels. In seconds (s).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|IdleLock")
		float IdleLockTime = 1.5f;

	bool bIsIdleLocked = false;

	float IdleLockCount = 0;

	float IdleLockSpeedThreshold = 1.f;

#pragma endregion

#pragma region Debug

	void DebugSuspension();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|Debug")
		bool bDebugSuspension = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel|Debug")
		float SuspensionDebugLenghtMultiplier = 1.f;

#pragma endregion


	float GameTime = 0.f;
	
	float SteeringInput = 0.f;
	float SensitvityInput = 0.f;
	bool bHandbrakeInput = false;
	float BrakeInput = 0.f;

	UStaticMeshComponent* WheelMeshComponent;

	float SteeringRotation = 0.f;
	float WheelRelativeOffset = 0.f;
	FRotator WheelLocalRotation = FRotator::ZeroRotator;
	
	FTransform physicsTransform {};
	FVector LinearVelocity = FVector::ZeroVector;
	FVector Start {};
	FVector up {};
	FVector right {};
	FVector Acceleration {};

	UPrimitiveComponent* VehiclePrimitiveComponent;
	FCollisionShape SweepShape;
	float WheelRadius = 0.315f;

	FHitResult Hit;
	float CurrentLength = 40.f;
	float GroundDistance = 0.f;
	bool bSingleRaycast = false;
	float CurrentCompression = -1.f;	

	float CurrentLateralForceCut = 1.f;
};