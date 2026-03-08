// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PDWMinigameMovingMesh.generated.h"

class UPDWInteractionBehaviour;
class APDWPlayerController;
class UPDWMovingObjectBehaviour;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PDW_API UPDWMinigameMovingMesh : public UStaticMeshComponent
{
	GENERATED_BODY()


public:
	UPDWMinigameMovingMesh();

	virtual void PossesComponent(APDWPlayerController* inController,UPDWMovingObjectBehaviour* MovingBehaviour = nullptr);
	virtual void UnpossesComponent(bool bUnInitializingBehaviour = false,APDWPlayerController* inController = nullptr);
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Reset();
	void SetID(FName inID) { ID = inID; }
	FName GetID() const { return ID; }

	void EndPlay(EEndPlayReason::Type Reason) override;
	bool GetPossessed() const;

	UPROPERTY(Editanywhere, Category = "Config")
	bool bClampMovingValue = false;

	UPROPERTY(Editanywhere,  Category = "Config", meta = (EditCondition = "bClampMovingValue",EditConditionHides))
	FFloatRange MinMaxHorizontalMovement;

	UPROPERTY(Editanywhere,  Category = "Config", meta = (EditCondition = "bClampMovingValue",EditConditionHides))
	FFloatRange MinMaxVerticalMovement;


	UPROPERTY(BlueprintReadWrite)
	APDWPlayerController* ControllerOwner = nullptr;
protected:

	bool bPossessed = false;

	FVector StartingPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Config")
	FName ID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Instanced,Category = "Config")
	TArray<UPDWInteractionBehaviour*> Behaviours;
	
	UPROPERTY()
	TArray<UPDWMovingObjectBehaviour*> CurrentMovingObjectBehaviours {};

	void BeginPlay() override;
};
