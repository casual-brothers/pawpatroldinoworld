// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "PDWTargetingComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class PDW_API UPDWTargetingComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UPDWTargetingComponent();

	UFUNCTION(BlueprintCallable)
	bool GetIsTargeted() const;
	UFUNCTION(BlueprintCallable)
	void SetIsTargeted(const bool bTarget);
	UFUNCTION(BlueprintCallable)
	bool IsFakeTarget() const { return bFakeTarget; }
	UFUNCTION(BlueprintCallable)
	void SetIsFakeTarget(bool bFake) {bFakeTarget = bFake; }
	
	UPROPERTY(EditAnywhere)
	bool bFakeTarget = false;
protected:

	bool bIsTargeted = false;
};
