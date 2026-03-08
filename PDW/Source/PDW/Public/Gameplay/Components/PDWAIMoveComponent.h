// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PDWAIMoveComponent.generated.h"

class APDWDinoPenArea;
class UNavigationSystemV1;
class UCharacterMovementComponent;
class UPDWInteractionReceiverComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWAIMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWAIMoveComponent();

	UFUNCTION(BlueprintCallable)
	void Init(FGameplayTag _PenTag, APDWDinoPenArea* _PenAreaRef);

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 50.f;

	UPROPERTY(EditAnywhere)
	 float StopDistanceFromPlayers = 100.f;

	 UPROPERTY(EditAnywhere)
	 float StopDistanceFromVehicles = 100.f;

	 UPROPERTY(EditAnywhere)
	 float RandomMoveTimer = 15.f;

	 UPROPERTY(EditAnywhere)
	 FGameplayTag SkeletalToCheck;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	virtual bool CanMove();

	UFUNCTION()
	virtual void Move();

	UPROPERTY()
	APawn* OwnerPawn = {};

	UPROPERTY()
	bool IsPenActive = false;

	UPROPERTY()
	FGameplayTag PenTag = {};

	UPROPERTY()
	FVector PenCenter = {};

	UPROPERTY()
	float PenRadius = 0.f;	

	UPROPERTY()
	UCharacterMovementComponent* CharMoveComp = {};

	UPROPERTY()
	UPDWInteractionReceiverComponent* InteractionComp = {};

	UPROPERTY()
	UAnimInstance* AnimInstance = {};

	UPROPERTY()
	UNavigationSystemV1* NavSys = {};

	UPROPERTY()
	float DefaultWalkSpeed = 0.f;
		
	UFUNCTION()
	void OnDinoPenActivityChange(const FGameplayTag _PenTag, bool IsActive);

	FTimerHandle MoveTimerHandle;	

	UPROPERTY()
	bool IsDoingAMontage = false;

	UFUNCTION()
	void OnMontageStart(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

};
