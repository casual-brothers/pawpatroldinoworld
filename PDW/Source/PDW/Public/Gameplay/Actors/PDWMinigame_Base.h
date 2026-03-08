// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Camera/CameraComponent.h"
#include "PDWMinigame_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMinigameSignature, APDWMinigame_Base*, Minigame);

class UFlowComponent;
class UPDWMinigameMovingMesh;
class UPDWMinigameConfigComponent;
class UPDWJigsawComponent;
class APDWPlayerController;

UCLASS(Blueprintable,BlueprintType)
class PDW_API APDWMinigame_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWMinigame_Base();

	UPROPERTY(BlueprintAssignable)
	FMinigameSignature MinigameFinished;


	virtual void Tick(float DeltaTime) override;

	UCameraComponent* GetMiniGameCamera() const { return CameraMinigame; }

	UFlowComponent* GetFlowComponent() const { return FlowComponent; }
	
	UFUNCTION(BlueprintCallable)
	UPDWMinigameMovingMesh* RequestMovingMeshComponent(APDWPlayerController* RequestingController);

	UFUNCTION(BlueprintCallable)
	void GetMovingMeshes(TArray<UPDWMinigameMovingMesh*>& OutMovingMesh);

	UFUNCTION(BlueprintCallable)
	UPDWMinigameConfigComponent* GetMinigameConfigComp();

	UPROPERTY()
	TArray<UPDWMinigameMovingMesh*> GivenMeshes;
protected:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	FName MultiPlayerMovingMeshTag = FName("MultipayerMovingMesh");

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	bool bUseHintTimer = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	float HintTimer = 10.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Components")
	UCameraComponent* CameraMinigame = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Components")
	USceneComponent* MinigameRoot = nullptr;

	UPROPERTY(EditAnywhere,Category = "Components")
	UFlowComponent* FlowComponent = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Components")
	UPDWInteractionReceiverComponent* InteractionReceiver = nullptr;

	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnMinigameLeft(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnMinigameInitializationComplete(const TArray<APDWPlayerController*>& Controllers);

	UFUNCTION()
	void TriggerHint();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnHintTriggered();

	UFUNCTION()
	void ResetHintTimer();

	UPROPERTY()
	TArray<UPDWMinigameMovingMesh*> MovingMeshList {};
	UPROPERTY()
	TArray<UPDWMinigameMovingMesh*> CachedMovingMeshList {};

	UPROPERTY()
	TArray<UPDWJigsawComponent*> PuzzleSlots {};

	UPROPERTY()
	UPDWMinigameConfigComponent* MiniGameComponent = nullptr;

private:
	UPROPERTY()
	UPDWMinigameMovingMesh* LastMovingMeshGiven = nullptr;

	UPROPERTY()
	UPDWMinigameMovingMesh* MultiplayerMovingMeshInstance = nullptr;

	UPROPERTY()
	TMap<APDWPlayerController*,UPDWMinigameMovingMesh*> LastControlledPiece {};

	FTimerHandle HintTimerHandle;
	
	UPROPERTY()
	TArray<AActor*> Actors;
};
