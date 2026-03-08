// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PDWPaleoCenterCustomization.generated.h"

class UFlowComponent;

UCLASS()
class PDW_API APDWPaleoCenterCustomization : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWPaleoCenterCustomization();

	UFUNCTION()
	UFlowComponent* GetFlowComponent() {return FlowComponent; };

	UFUNCTION()
	void ChangeCustomizationVisibility(bool IsVisible);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category = "Components")
	UFlowComponent* FlowComponent = nullptr;

};
