// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "PDWSplineRampComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PDW_API UPDWSplineRampComponent : public USplineComponent
{
	GENERATED_BODY()
	
	UPDWSplineRampComponent();

public:

	UFUNCTION(BlueprintCallable)
	void MoveOnSpline(const float inDeltaTime);
	UFUNCTION(BlueprintCallable)
	void SetLogicEnabled(bool bInActive);
	UFUNCTION(BlueprintCallable)
	void SetRampUser(AActor* inActor);
	UFUNCTION(BlueprintCallable)
	bool GetLogicEnabled() const;
	UFUNCTION(BlueprintCallable)
	AActor* GetRampUser() const;
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

protected:

	UFUNCTION()
	void Jump(const float& inJumpValue);

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void StopRampUsage(AActor* Actor);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Config")
	float RampSpeed = 600.0f;

private:
	float JumpValue = 0.0f;
	UPROPERTY()
	AActor* RampUser = nullptr;
	bool bLogicEnabled = false;
	float CurrentDistance = 0.0f;
};
