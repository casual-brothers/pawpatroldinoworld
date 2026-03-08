// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "PDWMovingObjectBehaviour.generated.h"

class UCameraComponent;
class UPDWMinigameMovingMesh;
class APDWMinigame_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectChange, UPDWMinigameMovingMesh*, OldMovingMesh, UPDWMinigameMovingMesh*, NewMovingMesh);

UENUM(BlueprintType)
enum class EMiniGameActorSource : uint8
{
	PossesPup =						0,
	PossesSceneComponent =			1,
	PossesSpecificClass =			2,
};

UENUM(BlueprintType)
enum class EMovingPlane : uint8
{
	XY =						0,
	XYZ =						1,
	XZ =						2,
};

USTRUCT(BlueprintType)
struct PDW_API FMovingObjectBehaviourConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Configuration")
	EMiniGameActorSource PossesRule = EMiniGameActorSource::PossesSceneComponent;

	UPROPERTY(Editanywhere, Category = "Configuration", meta = (EditCondition = "PossesRule == EMiniGameActorSource::PossesSceneComponent",EditConditionHides))
	float Speed = 100.0f;

	UPROPERTY(Editanywhere, Category = "Configuration", meta = (EditCondition = "PossesRule == EMiniGameActorSource::PossesSceneComponent",EditConditionHides))
	bool bMultiplePosses = false;

	UPROPERTY(Editanywhere, Category = "Configuration", meta = (EditCondition = "PossesRule == EMiniGameActorSource::PossesSceneComponent",EditConditionHides))
	EMovingPlane MovingRules = EMovingPlane::XZ;

	UPROPERTY(Editanywhere, Category = "Configuration", meta = (EditCondition = "PossesRule == EMiniGameActorSource::PossesSpecificClass",EditConditionHides))
	TSubclassOf<APawn> SpecificClassToPosses = nullptr;

	UPROPERTY(EditAnywhere,Category = "Configuration")
	bool bNotifySuccess = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag TargetIDToNotify = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag EventID = FGameplayTag::EmptyTag;
};

UCLASS()
class PDW_API UPDWMovingObjectBehaviour : public UPDWMiniGameInputBehaviour
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnObjectChange OnObjectChange;

	void InitializeBehaviour(APDWPlayerController* inController,UPDWMinigameConfigComponent* inMiniGameComp) override;
	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;
	void UninitializeBehaviour() override;
	virtual void RequestSceneComp(APDWPlayerController* inController);

	UFUNCTION(BlueprintCallable)
	UPDWMinigameMovingMesh* GetMovingMeshComponent();
	
	UFUNCTION(BlueprintCallable)
	APawn* GetPossesedPupInstance();

	UFUNCTION(Blueprintpure)
	const FInstancedStruct GetMovingObjectBehaviourConfig() {return MovingObjectBehaviourConfig;};

	UFUNCTION(BlueprintPure)
	const EMiniGameActorSource GetMinigamePossesRule();

	UFUNCTION(BlueprintPure)
	const bool HaveMinigameMultiplePosses();

protected:

	UPROPERTY(EditAnywhere,Category = "Configuration",meta = (BaseStruct = "/Script/PDW.MovingObjectBehaviourConfig"))
	FInstancedStruct MovingObjectBehaviourConfig;
	UFUNCTION(BlueprintCallable)
	void SetPossesedPupInstance(APawn* inPup);
	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetCameraComponent();

	virtual void InitializeSceneComponentPosses(APDWPlayerController* inController);

	UFUNCTION()
	void OnExternalBindEvent(const FGameplayTag& ActionToBind,APDWPlayerController* InOwner);

	UPROPERTY()
	UPDWMinigameMovingMesh* SceneComponentInstance = nullptr;

	UPROPERTY()
	APDWMinigame_Base* MiniGame = nullptr;

	UPROPERTY()
	APawn* PossessedPupInstance = nullptr;

	UPROPERTY()
	UCameraComponent* CameraComp = nullptr;
private:

};
