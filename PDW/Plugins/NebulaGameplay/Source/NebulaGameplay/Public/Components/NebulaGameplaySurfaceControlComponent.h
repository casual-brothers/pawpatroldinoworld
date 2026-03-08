
#pragma once

#include "Components/SceneComponent.h"
#include "Chaos/ChaosEngineInterface.h"
#include "NebulaGameplaySurfaceControlComponent.generated.h"



UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NEBULAGAMEPLAY_API UNebulaGameplaySurfaceControlComponent : public USceneComponent
{
	GENERATED_BODY()

public:

public:

	UNebulaGameplaySurfaceControlComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void ActivateSurfaceComponent(bool inActive) { bIsSurfaceComponentActive = inActive; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		bool IsOnGround() { return bIsOnGround; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		TEnumAsByte <EPhysicalSurface> GetCurrentGroundSurface() { return CurrentSurface; }

protected:
	
	void SetIsOnGround(bool bGrounded);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
		TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
		FVector StartPointOffset = FVector();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
		float CollisionTraceLength = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|ParallelBehavior")
		bool bForceActorParallelToGround = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|ParallelBehavior")
		float ActorForwardTraceDelta = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|ParallelBehavior")
		float RotationSmoothVelocity = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|ParallelBehavior")
		float MaxRoll = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|ParallelBehavior")
		float MaxPitch = 90.f;
	
	UPROPERTY()
		float ImpactZPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|ParallelBehavior")
		TArray<TSubclassOf<AActor>> ActorsToIgnore{};


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Debug")
		bool bShowDebugInfos = false;

	UPROPERTY()
		AActor* OwnerActor = nullptr;

	UPROPERTY()
		TEnumAsByte <EPhysicalSurface> PreviousSurface = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY()
		TEnumAsByte <EPhysicalSurface> CurrentSurface = EPhysicalSurface::SurfaceType_Default;

	bool bIsSurfaceComponentActive = true;

	virtual void UpdateSurfaceComponent(float deltatime);

	virtual void HandleSurfaceChange();

	virtual void HandleOnGroundedChange();

	bool bIsOnGround = false;

	FRotator TargetParallelRotator;

};
