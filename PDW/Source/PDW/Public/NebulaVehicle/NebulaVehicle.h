#pragma once

#include "Curves/CurveFloat.h"
#include "DrawDebugHelpers.h"
#include "CoreMinimal.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "NebulaVehicle.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NEBULAVehicleLog, Log, All);

class UNebulaWheel;
class UNebulaSuspensionComponent;

UENUM(BlueprintType)
enum class ENebulaSpeedClamp : uint8
{
	NoClamp UMETA(Display = "No Clamp"),
	Constant UMETA(Display = "Constant"),
	Linear UMETA(Display = "Linear"),
	Curve UMETA(Display = "Curve")
};

USTRUCT(BlueprintType)
struct FNebulaRangedClamp
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Custom Data")
	FVector2D InputRange = FVector2D::Zero();

	UPROPERTY(EditAnywhere, Category = "Custom Data")
	FVector2D OutputRange  = FVector2D::Zero();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNebulaOnVehicleTakeOff);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNebulaOnVehicleLanding);

UCLASS()
class ANebulaVehicle : public APawn
{
	GENERATED_BODY()

public:
	ANebulaVehicle();

	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime);

	virtual void Tick(float DeltaTime) override;

	virtual void PhysicsGameTick(float DeltaTime);

#pragma region Setters

	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		void OverrideRPM(float newRPM);

	//Resume physics wake state
	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		void RTWakeRigidBodies();

	void SetMaxSpeed(float Soft, float Hard, bool Reset = false);

	UFUNCTION(BlueprintCallable)
	void SetMaxSpeed(float value) { MaxSpeed = value; }

	void SetIsOnAir(bool value, float DeltaTime);

	void SetSteeringValue(float value) { DriftTorque = value; }

	UFUNCTION(BlueprintCallable)
	void SetSteeringMultiplier(float value) { SteeringMultiplier = value; }
	UFUNCTION(BlueprintCallable)
	void SetAccelerationValue(float value) { EngineTorque = value; }
	UFUNCTION(BlueprintCallable)
	void SetAccelerationMultiplier(float value);

#pragma endregion

#pragma region Getters

	//Returns the speed of the vehicle in kilometers per hour.
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetSpeedKPH();

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetVerticalSpeedKPH();
		//Returns the local velocity vector in meters per second (m/s).
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		FVector GetVelocityVector();

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetThrottleInput();

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetBrakesInput();

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetSteeringInput();

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		bool GetHandbrakeInput();

	// Returns the current engine torque.
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetEngineCurrentTorque();

	// Returns the current RPM.
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		virtual float GetRPM();

	//Returns the async tick physics transform of the vehicle.
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		FTransform GetPhysicsTransform();

	//Returns the angle between the vehicle's forward direction and velocity angle, in degrees.
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetSideSlipAngle();

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		EPhysicalSurface GetWheelMaterial(int WheelIndex);

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetTimePassedBreakingScaled() { return TimePassedBreakingScaled; }

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetTimePassedBreaking() { return TimePassedBreaking; }

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetTimePassedSteeringScaled() { return TimePassedSteeringScaled; }

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetTimePassedSteering() { return TimePassedSteering; }

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		UPrimitiveComponent* GetPrimitiveComponent() { return PrimitiveComponent; }

	FBodyInstance* GetBodyInstance() { return AgentBodyInstance; }
	
	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		bool IsOnAir() { return bOnAir; }

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		float GetOnAirTime() { return OnAirTime; }

	UFUNCTION(BlueprintPure, Category = "NebulaVehicle")
		FVector GetAngularSpeed() { return BodyAngolarSpeed; }

#pragma endregion

#pragma region Input

	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		void SetThrottleInput(float value);

	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		void ApplyHandbrakeInput(bool value);

	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		void SetBrakeInput(float value);

	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		void SetSteeringInput(float value);
	float GetProcessedSteeringInput(float value);
	void AnimateSteering();
	void StopCar(bool Immediatly = true);
	UFUNCTION(BlueprintCallable)
	void StopCarBP(FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, bool Immediatly = false);

#pragma endregion

#pragma region Events
	
	FNebulaOnVehicleTakeOff OnVehicleTakeOff;
	FNebulaOnVehicleLanding OnVehicleLanding;

#pragma endregion

protected:

	virtual void BeginPlay() override;

	virtual void UpdatePhysicsCore(float DeltaTime);

	virtual void UpdateEngineArcade(float DeltaTime);
	virtual void UpdateSteering(float DeltaTime) {};
	virtual void UpdateDrag(float DeltaTime);

	virtual void CheckIsOnAir(float DeltaTime);

	TArray<TObjectPtr<UNebulaSuspensionComponent>> SuspensionComponents {};

#pragma region ForceMoveVehicle

	bool bStopCar = false;
	bool bStopCarImmediatly = false;
	bool bTeleportCar = false;
	FVector TeleportLocation {};
	FRotator TeleportRotation {};
	
	void TeleportCar(FVector Location, FRotator Rotation);

#pragma endregion

#pragma region Air

	int OnAirWheel = 0;
	// This controls the in air condition where forces and torques cannot be applied. This variables controls how many wheels have to be off the ground for the vehicle to be considered airborne.
	// When the vehicle is airborne forces and torques are no longer applied.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|Air")
		int OnAirWheelCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|Air")
		bool bAccelerateOnAir = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|Air")
		bool bSteerOnAir = false;

#pragma endregion

#pragma region Engine

	float OriginalMaxSpeed = 0.f;

	float AccelerationMultiplier = 1.f;
	
	//In Newton-Meters. This is the maximum torque that the engine will produce.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Engine")
		float EngineTorque = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Engine")
		float MaxSpeed = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Engine")
		float MaxAscensionSpeed = 350.f;

	// This is a normalized torque curve which should be from 0 to 1. The delivered engine torque will be the value at a point on this curve multiplied by the EngineTorque.
	// X axis: RPM/Speed
	// Y axis: Torque multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Engine")
		TObjectPtr<UCurveFloat> EngineTorqueCurve;

	// This affects how quickly the RPM decreases when the throttle is no applied. Higher values result in a quicker decrease. Lower values result in less off-throttle deceleration.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Engine", meta = (ClampMin = "0"), meta = (ClampMax = "1"), meta = (EditCondition = "EngineType == ENebulaBehaviourType::Standard", EditConditionHides))
		float EngineBrakeCoefficient = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Engine")
		float MaxAccelerationMultiplier = 3.f;

#pragma endregion

#pragma region Brake

	bool bAutoBrakeActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Brakes")
		float AutoBrakeForce = 15000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Brakes")
		TObjectPtr<UCurveFloat> BrakeForceCurveOverSpeed {};

	float TimePassedBreaking = 0.f;
	float TimePassedBreakingScaled = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Brakes")
		bool bUseBrakeAsReverse = true;

	//This is the maximum speed (KPH) that can be achieved in reverse if UsedBrakesAsReverse is used. If not the maximum speed in reverse will be determined by the gear ratio set.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Brakes")
		float MaxSpeedInReverse = 60.f;

#pragma endregion

#pragma region Handling

	float TimePassedSteering = 0.f;
	float TimePassedSteeringScaled = 0.f;

	float SteeringMultiplier = 1.f;

	// This is an assistance/helper torque (in Unreal Torque Units) that is tied to steering input and can be used to increase the rotation rate of the vehicle.
	// It is fully compliant with the rest of the vehicle behaviour and will conform to all of it's other systems.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Handling")
		float DriftTorque = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Handling")
		float AdditionalSteeringTorque = 1.f;

	// Steering input decrease as a function of this curve. The x axis is speed, the y axis is the maximum steering value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Handling")
		TObjectPtr<UCurveFloat> CurveSteeringInputClamp;

	// Constant sensitivity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Handling")
		float SteeringSensitivity = 5.f;

#pragma endregion

#pragma region Aerodynamics

	// This is a dimensionless scalar. Higher values result in more drag. Generally this value ranges from 0.25-0.4 for most road cars.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Aerodynamics")
		float CoefficientOfDrag = 0.3f;

	//Frontal cross sectional area. In meters squared. m^2
	//This is the area that drag acts on. Higher values will result in more drag.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Aerodynamics")
		float CrossSectionArea = 1.5872454f;

	//This is the atmosphere air density used drag force calculations. Default value is the air density at sea level. In kg/m^3.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NebulaVehicle|Aerodynamics")
		float AirDensity = 1.225f;

#pragma endregion

#pragma region IdleLock

	bool bIsIdleLocked = false;
	
	// This system is used to hold a vehicle in place when stationary or travelling very slowly after a certain amount of time and prevent micro slipping. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|IdleLock")
		bool bUseIdleLock = true;

	// How long to wait (in seconds) to lock vehicle in place.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|IdleLock")
		float IdleLockTime = 1.5f;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|IdleLock")
		float IdleLockCount = 0;

	// Speed at which to begin transitioning to idle locked state.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|IdleLock")
		float IdleLockSpeedThreshold = 10.f;

	// This is the force used on inclines to prevent slipping and micro slipping. If the force is too high it may cause an impulse and move the vehicle out of the idle locked state.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|IdleLock")
		float IdleLockStiffness = 3000.f;

	bool bIsIdleLockedRotationLocation = false;
	float IdleLockRotationLocationCount = 0.f;

#pragma endregion

	//This controls whether to automatically wake rigid body components. Disabling this option can resolve issues with having lots of replicated vehicles.
	//If disabled use the function 'RTWakeRigidBodies' to resume wake state
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaVehicle|Physics|Misc")
		bool bAutoWakeRigidBodies = true;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UPrimitiveComponent> PrimitiveComponent;

	float AsyncDeltaTime = 0.f;
	float GameDeltaTime = 0.f;

	float RPM = 0.f;
	float CurrentTorque = 0.f;

#pragma region PhysicalData

	float VelocityDirection {};
	FVector ForwardVector {};
	FVector RightVector {};
	FVector UpVector {};

#pragma endregion

#pragma region GearboxDelay

	FTimerHandle ShiftDelayTimerHandle;
	bool ShiftTimerActive = false;
	bool IsUpshifting = false;
	bool IsDownshifting = false;
	int UpshiftFrameCount = 0;
	int DownshiftFrameCount = 0;

	float NETWORK_TEMP_X = 0;

#pragma endregion

#pragma region Auxiliary

	bool bOnAir = false;

	float OnAirTime = 0.f;

#pragma endregion

#pragma region Input

	float Throttle = 0.f;

	float Brakes = 0.f;

	float SteeringInput = 0.f;

	bool bHandbrakeInput = false;

#pragma endregion

public:

	float BodyMass {};
	FTransform BodyTransform {};
	FVector BodyForward {};
	FVector BodyRight {};
	FVector BodyUp {};
	FVector CentreOfMass {};
	FVector BodyLocation {};
	FQuat BodyRotation {};
	FVector BodyLinearSpeed {};
	FVector PreviousBodyLinearSpeed {};
	FVector BodyAngolarSpeed {};
	FVector BodyLinearAcceleration {};

	FBodyInstance* AgentBodyInstance {};
	Chaos::FRigidBodyHandle_Internal* BodyHandle {};

	void AddForce(FVector Force, FString DebugName = FString());
	void AddTorque(FVector Torque, bool bAccelChange = false, FString DebugName = FString());
	void AddForceAtLocation(FVector Force, FVector Location, FString DebugName = FString());
	void AddImpulse(FVector Impulse, FString DebugName = FString());
	void AddImpulseAtLocation(FVector Impulse, FVector Location, FString DebugName = FString());
	FTransform GetPhysTransform();
	FVector GetLinearVelocity();
	FVector GetLinearVelocityAtPoint(FVector Point);
	FVector GetAngularVelocity();
	UFUNCTION(BlueprintCallable, Category = "NebulaVehicle")
		FVector GetLinearAcceleration();
	FVector GetCOMLocation();
	void SetLinearVelocity(FVector NewLinVelocity, bool bAddToCurrent);
	void SetAngularVelocityInRadians(FVector NewAngVelocity, bool bAddToCurrent);
	void SetAngularVelocityInDegrees(FVector NewAngVelocity, bool bAddToCurrent);
	void SetWorldLocationAndRotation(FVector Location, FRotator Rotation);

	void CalculateVehicleStateVariable(float DeltaTime);
};
