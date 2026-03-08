
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NebulaGameplayFreeCamera.generated.h"

class USphereComponent;
class UInputComponent;
class UCameraComponent;

UCLASS()
class NEBULAGAMEPLAY_API ANebulaGameplayFreeCamera : public APawn
{
	GENERATED_BODY()

public:

	ANebulaGameplayFreeCamera(const FObjectInitializer& ObjectInitializer);


	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void InitializeFreeCamera(AActor* ActorTarget, FTransform InitialTransform);

	void ToggleUI(bool bEnable);
	void UpdateFollowActor(AActor* ActorTarget);
	void SwitchFollowActor();
	void SwitchCamera();
	void SwitchDpadUsage();
	void SwitchUfoMode();
	void ResetRoll();
	//Input

	UFUNCTION(BlueprintCallable, Category = "FreeCamera")
		void SetInputEnabled(bool inEnabled) { bCameraInputEnabled = inEnabled; }

	static const FName FREECAMERA_TAG;

	//Components
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FreeCamera")
		UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FreeCamera")
		USphereComponent* SphereComponent = nullptr;
				
	UPROPERTY()
		bool bSwitchCamera = false;

	UPROPERTY()
		bool bSwitchDpadUsage = false;

protected:
	//Input Bindings

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName LateralMovementInputName = FName("FreeCameraLateralMovement");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName LongitudinalMovementInputName = FName("FreeCameraLongitudinalMovement");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName UpMovementInputName = FName("FreeCameraUpMovement");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName DownMovementInputName = FName("FreeCameraDownMovement");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName YawRotationInputName = FName("FreeCameraYawRotation");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName PitchRotationInputName = FName("FreeCameraPitchRotation");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName RollClockWiseRotationInputName = FName("FreeCameraRollClockWiseRotation");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName RollAntiClockWiseRotationInputName = FName("FreeCameraRollAntiClockWiseRotation");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraFreezeTimeInputName = FName("FreeCameraFreezeTime");
		
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraUfoModeInputName = FName("FreeCameraUfoMode");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraFollowTargetInputName = FName("FreeCameraFollowTarget");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraUpDpadInputName = FName("FreeCameraUpDpad");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraDownDpadInputName = FName("FreeCameraDownDpad");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraRightDpadInputName = FName("FreeCameraRightDpad");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraLeftDpadInputName = FName("FreeCameraLeftDpad");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraResetRollInputName = FName("FreeCameraResetRoll");
		
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraSwitchCameraInputName = FName("FreeCameraSwitchCamera");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FreeCamera|Input")
		FName FreeCameraSwitchDpadInputName = FName("FreeCameraSwitchDpad");


	//Tuning

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MovementSpeed = 500.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MinMovementSpeed = 50.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MaxMovementSpeed = 10000.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MovementSpeedMultiplier = 1.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		bool bVerticalMovementRelativeToWorld = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float RotationSpeed = 50.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float FovSpeed = 25.f;
				
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MaxDof = 10000.f;
		
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float InitialDof = 788.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float DofSpeed = 1.f;
						
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MaxSensorWidth = 10000.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float SensorWidthSpeed = 1.f;
				
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float InitialSensorWidth = 1135.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MinFov = 10.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MaxFov = 170.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		bool bCheckdistanceFromTarget = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Movement")
		float MaxDistanceFromTarget = 1500.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Collision")
		bool bDoCollisionCheck = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Collision")
		float CollisionSphereRadius = 30.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Collision")
		int32 CollisionCheckMaxIterations = 10;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Collision")
		float CollisionCheckCorrection = 10.f;

	// If the single axis dead zone in the input settings is higher, the higher value is used for that axis
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Collision", meta = (ClampMin = "0.0"))
		float AxisDeadzone = 0.05f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Time")
		float MinTimeSpeed = 1 / 60.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Time")
		float MaxTimeSpeed = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FreeCamera|Tuning|Time")
		float TimeSpeedMultiplier = 5.f;
		
	UPROPERTY()
		bool bFollowTarget = true;

	UPROPERTY()
		bool bUfoMode = false;
		

private:

	UPROPERTY()
		AActor* CameraTarget = nullptr;
				
	FVector CurrentRelativeLocation;

	UPROPERTY()
		UInputComponent* InputComponentRef = nullptr;

	bool bCameraInputEnabled = true;

	//MovementHandlingFunctions

	void HandleMovement(float DeltaTime);
	void HandleRotation(float DeltaTime);
	void HandleZoom(float Deltatime);
	void HandleTime(float Deltatime);


	void SwitchFreezeTime();

	//InitialValues

	float InitialFov;

	void ResolveCollisions(FVector DesiredOffset);

	void RemoveAllIMCFromController();
	//Internal Values

	bool bCollisionInitialized = false;

	FVector LastValidPosition = FVector::ZeroVector;

	bool bFreezeTime = false;
	float TimeScale = 1.0f;

};