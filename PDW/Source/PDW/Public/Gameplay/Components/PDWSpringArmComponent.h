// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "PDWSpringArmComponent.generated.h"

class APDWCharacter;
class UCurveFloat;
class UCameraComponent;

UCLASS(Blueprintable)
class UPDWCameraParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	// General parameter
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TransformLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TransformRotation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NormalArmLength = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultFOV = 60.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "TODO"))
	UCurveFloat* AdditionalFOVOverSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> ProbeChannel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LookTarget", meta = (ToolTip = "The point to look at"))
	FVector TargetDefaultOffset = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LookTarget", meta = (ToolTip = "How much should I look down when falling"))
	float LookWhenFallingAmount = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LookTarget", meta = (ToolTip = "Clamp the max amount to look up and down"))
	FVector2D LookWhenFallingRange = FVector2D(-500.0f, 500.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LookTarget", meta = (ToolTip = "Interpolation speed"))
	FVector LookTargetInterSpeed = FVector();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LookTarget", meta = (ToolTip = "Interpolation speed"))
	UCurveFloat* LookTargetZCurve{};
	// Lag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "How fast is the camera to reach the transform rotation"))
	FVector RotationalSpeedVector = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "How fast is the camera to reach the transform rotation during teleport"))
	FVector LinearTeleportLagLocation = FVector(0, 0, 0);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "How fast is the camera to reach the transform rotation"))
	FVector LinearLagVector = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "How fast is the camera to reach the transform rotation"))
	UCurveFloat* LinearLagZCurve {};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "TODO"))
	UCurveFloat* CameraLagRotationCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "TODO"))
	float BackMovementInputDeadZoneValue = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lag", meta = (ToolTip = "TODO"))
	float StandingPupCameraSpeedMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManualCamera", meta = (ToolTip = "TODO"))
	bool ManualCamera = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManualCamera", meta = (ToolTip = "TODO"))
	float ManualCameraInputSpeed = 2.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManualCamera", meta = (ToolTip = "TODO"))
	float ManualCameraNoInputTimeReset = 2.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManualCamera", meta = (ToolTip = "TODO"))
	float ManualCameraNoInputResetSpeed = 2.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SplitScreen")
	float SS_MultiplierFOV = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SplitScreen")
	float SS_MultiplierArmLenght = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDebug = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDebugCollisions = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDebugLookAt = false;
};



UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UPDWSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	UPDWSpringArmComponent();
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetPDWCharacter(APDWCharacter* Character);
	void SetAttachedCamera(UCameraComponent* Camera);
	void Init();
	UCameraComponent* GetAttachedCamera() const { return AttachedCamera; }

	FVector GetForwardMovementVector();
	FVector GetRightMovementVector();

	UFUNCTION(BlueprintCallable)
	void SetManualCameraActive();
	UFUNCTION(BlueprintCallable)
	void SetAutomaticCameraActive();

	UFUNCTION(BlueprintPure)
	UPDWCameraParameters* GetCurrentCameraParameters() { return CurrentCameraParameters; };

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGameplayStateEnter();

	void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	void UpdateArmLength(float DeltaTime);
	void RotateCamera(float DeltaTime);
	void UpdateFOV(float DeltaTime);

	UFUNCTION()
	void OnTeleportTriggered();
	UFUNCTION()
	void OnTeleportCompleted();

	UFUNCTION()
	void OnMovementInputChanged(FVector2D NewInput);
	UFUNCTION()
	void OnCameraInputChanged(FVector2D NewInput);

	UFUNCTION()
	void ResetManualCamera();

	UFUNCTION()
	void OnMultiplayerStateChange();

protected:
	UPROPERTY(EditAnywhere, Category = "DESIGN")
	TObjectPtr<UPDWCameraParameters> AutomaticCameraParameters = nullptr;
	UPROPERTY(EditAnywhere, Category = "DESIGN")
	TObjectPtr<UPDWCameraParameters> ManualCameraParameters = nullptr;

	UPROPERTY()
	TObjectPtr<UPDWCameraParameters> CurrentCameraParameters = nullptr;

	UPROPERTY()
	TObjectPtr<APDWCharacter> PDWCharacter = nullptr;

	UPROPERTY()
	UPawnMovementComponent* PDWCharacterMoveComp = nullptr;

	UPROPERTY()
	TObjectPtr<UCameraComponent> AttachedCamera = nullptr;

	FVector LastTargetDefaultOffset = FVector::ZeroVector;
	bool bSplitScreenMode = false;
	bool bCameraShakeEnabled = true;
	float LastCameraInput = 0;
	float AccumulatedCameraInput = 0;
	FVector2D LastMovementInput;

	UPROPERTY()
	FTimerHandle CameraResetHandle;

	UPROPERTY()
	FVector CameraLagLocationVectorToUse;

	FTimerHandle ResetCameraLagHandle;
};
