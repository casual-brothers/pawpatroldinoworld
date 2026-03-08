// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"
#include "PDWCustomizationLocation.generated.h"

class UCameraComponent;

UCLASS()
class PDW_API APDWCustomizationLocation : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	APDWCustomizationLocation();

	UFUNCTION(BlueprintPure)
	UCameraComponent* GetCustomizationCameraActor() {return CustomizationCamera;};

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CustomizationCamera;

	void BeginPlay() override;
};
