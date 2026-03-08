// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PDWCollectibleBase.generated.h"

class UPDWPickUpComponent;
class APDWPlayerController;
class UFlowComponent;

UCLASS()
class PDW_API APDWCollectibleBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDWCollectibleBase();

	UPROPERTY(EditAnywhere)
	UPDWPickUpComponent* PickUpComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* CollectibleMeshComponent = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PickUpEnd();

protected:

	UPROPERTY(BlueprintReadWrite)
	bool AlreadyTaken = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPickUpStart(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController);

	UFUNCTION()
	virtual void OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController);
};
