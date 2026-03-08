// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PDWAimingTargetActor.generated.h"

UCLASS()
class PDW_API APDWAimingTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWAimingTargetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsTargetAquired() const { return bTargetAquired; }
	UFUNCTION(BlueprintCallable)
	void SetTargetAquired(bool bAquired) { bTargetAquired = bAquired; }

	UFUNCTION(BlueprintCallable)
	bool IsFakeTarget() const { return bFakeTarget; }
	UFUNCTION(BlueprintCallable)
	void SetIsFakeTarget(bool bFake) {bFakeTarget = bFake; }

private:

	bool bTargetAquired = false;
	bool bFakeTarget=false;
};
