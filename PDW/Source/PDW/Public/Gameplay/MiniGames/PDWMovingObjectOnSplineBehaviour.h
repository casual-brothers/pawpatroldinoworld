// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "PDWMovingObjectOnSplineBehaviour.generated.h"

class USplineComponent;
class UPDWPreyComponent;
class USpringArmComponent;

USTRUCT(BlueprintType)
struct PDW_API FMovingObjectOnSplineConfig : public FMovingObjectBehaviourConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 StartingSplineIndex{ 0 };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FGameplayTag> SplineTags{};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float SpeedOnSpline{ 300.0f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bOverridePupSpeed{ false };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(EditCondition ="bOverridePupSpeed",EditConditionHides))
	FGameplayTag MainMeshTag{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float TransitionToSplineTime{ 0.5f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PushBackDuration{ 0.5f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PushbackDistance{ 500.0f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DeltaZOnSpline = 150.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bSpawnPrey{ false };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "TraceBoxConfig")
	float TraceBoxHalfSize{ 50.0f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "TraceBoxConfig")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes = { /*UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)*/ };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "TraceBoxConfig")
	FVector BoxSize{ FVector::ZeroVector };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "TraceBoxConfig")
	bool bDebugTrace{ false };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CameraConfig")
	float CameraSpeed{ 300.0f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CameraConfig")
	float CameraDeltaSpeed{ 50.0f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CameraConfig")
	FFloatRange AcceptableCameraSpeedRange;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CameraConfig")
	int32 DefaultSplineIndex{ 0 };

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CameraConfig")
	float MinCameraDistanceFromPlayer{ 1000.0f };

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (EditCondition = "bSpawnPrey",EditConditionHides))
	TSubclassOf<AActor> PreyClass{ nullptr };

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCollisionProfileName ObstacleCollisionProfile;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCollisionProfileName RampCollisionProfile;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAllowJumpEverywhere{ false };
};

UCLASS()
class PDW_API UPDWMovingObjectOnSplineBehaviour : public UPDWMovingObjectBehaviour
{
	GENERATED_BODY()
	
public:

	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;

	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;

	void CustomTick(const float inDeltaTime) override;

	void UpdateCameraPosition(const float inDeltaTime);

	void UninitializeBehaviour() override;

	UFUNCTION(BlueprintCallable)
	float GetCurrentDistance() const {return CurrentDistance;};

protected:

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void Jump(const float& inJumpValue);

	UFUNCTION()
	virtual void StopJump();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_BeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	TMap<int32,USplineComponent*> SplineComponents = {};

	int32 CurrentSplineIndex = 0;

	bool CheckSideBeforeSwitchLane(const int32& inSelectedIndex);
	
	UFUNCTION()
	void SetRamModeActive();
	UFUNCTION()
	void SetRamModeInactive();

private:

	//JUMP	
	float JumpValue = 0.0f;
	bool bJumping =false;
	bool bWaitForReset = false;
	//CAMERA
	float CameraSpeed = 0.0f;
	float CurrentCameraPosition = 0.0f;
	int32 DefaultSplineIndex = 0;
	float MinCameraDistanceFromPlayer = 0;
	float DeltaZOnSpline = 0.0f;
	float SplinesOffsetDistance = 0.0f;
	float CameraDeltaSpeed = 0.0f;

	float CurrentDistance = 0.0f;
	float CachedSpeed = 0.0f;
	float CachedTransitionDuration = 0.0f;
	float TransitionTimer = 0.0f;

	float PushbackTimer = 0.0f;
	float CachedPushbackDuration = 0.0f;
	float PushbackStartDistance = 0.0f;
	float PushbackTargetDistance = 0.0f;
	float CachedPushDistance = 0.0f; 

	bool bRunCompleted = false;
	bool bIsTransitioning = false;
	bool bIsPushedBack = false;
	bool bStopLogic = false;
	bool bCameraShouldUpdate = true;
	bool bRamModeActive = false;

	FVector TransitionStartLocation;
	FVector TransitionEndLocation;
	FRotator TransitionStartRotation;
	FRotator TransitionEndRotation;
	int32 TargetSplineIndex = 0;
	FFloatRange AcceptableCameraSpeedRange;

	void ResetVariables();

	UPROPERTY()
	USpringArmComponent* MinigameCameraComp = nullptr;

	UPROPERTY()
	TObjectPtr<UPDWPreyComponent> Prey = nullptr;

	UPROPERTY()
	AActor* CachedObstacle = nullptr;
};
