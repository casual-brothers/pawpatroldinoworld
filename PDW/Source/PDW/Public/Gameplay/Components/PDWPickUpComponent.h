// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PDWPickUpComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickUpStart, UPDWPickUpComponent*, Instigator, APDWPlayerController*, PlayerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUpConditionChange, UPDWPickUpComponent*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickUpEnd, UPDWPickUpComponent*, Instigator, APDWPlayerController*, PlayerController);

UCLASS()
class PDW_API UPDWPickUpComponent : public USphereComponent
{
	GENERATED_BODY()
	
	public:	
	// Sets default values for this component's properties
	UPDWPickUpComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FOnPickUpStart OnPickUpStart;

	UPROPERTY(BlueprintAssignable)
	FOnPickUpEnd OnPickUpEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	float PickUpDistance = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN") 
		UCurveFloat* MagneticCurve = nullptr;

	UPROPERTY()
		APawn* OverlappingChar;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	UPROPERTY()
		TArray<APawn*> InsideAreaPlayers;

	UPROPERTY()
		AActor* ActorOwner;

	UPROPERTY()
		float CurrentAnimationTime = 0.f;

	UPROPERTY()
		bool DoingPickUpAnimation = false;

	UPROPERTY()
		AActor* PickUpTarget = nullptr;

	UPROPERTY()
		APDWPlayerController* PickUpTargetController = nullptr;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void StartPickUp(AActor* OverlappingCharacter);

	UFUNCTION()
		void PickUpAnimation(float DeltaTime);

	UFUNCTION()
		void EndPickUp();

public:

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
