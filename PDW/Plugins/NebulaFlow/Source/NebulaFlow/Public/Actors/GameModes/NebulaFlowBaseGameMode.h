#pragma once

#include "GameFramework/GameMode.h"

#include "NebulaFlowBaseGameMode.generated.h"

class ANebulaGameplayFreeCamera;
class APlayerController;

UCLASS()
class NEBULAFLOW_API ANebulaFlowBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ANebulaFlowBaseGameMode();

	virtual void CreateSpectator(){};
	virtual void CreatePlayer2(int32 UserIndex){};
		
public:

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ANebulaGameplayFreeCamera> FreeCameraPawn;

	void EnableFreeCamera();
	
	void UpdateTargetActorFreeCamera();

	void DisableFreeCamera();

	void DestroyPlayerCamera();

	//void RemoveAllIMCFromController(APlayerController* PlayerController);
	virtual void SetInitialFreeCameraLocationAndRotation(FVector& OutLocation, FRotator& OutRotation);

protected:
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(Transient)
		APlayerController* FreeCameraController = nullptr;
	
 	int32 CameraPlayerIndex = -1;
};