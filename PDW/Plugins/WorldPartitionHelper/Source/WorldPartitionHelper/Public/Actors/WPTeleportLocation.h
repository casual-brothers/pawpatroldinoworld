// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "WPTeleportLocation.generated.h"

UCLASS()
class WORLDPARTITIONHELPER_API AWPTeleportLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWPTeleportLocation();

	UFUNCTION(BlueprintCallable)
	void GetTeleportTransform(TArray<FTransform>& OutTransforms) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* PlayerTwoSpawnLocation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	FGameplayTag TeleportID = FGameplayTag::EmptyTag;
};
