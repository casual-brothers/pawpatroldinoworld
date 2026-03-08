// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WPHelperPoolingComponent.generated.h"

USTRUCT()
struct FAiControllerQueue
{
	GENERATED_BODY()

public:

	TArray<AController*> ControllerQueue;
};

USTRUCT(BlueprintType)
struct FAIControllerPoolSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditdefaultsOnly, Category = "PoolingSettings")
	int32 PoolSize = 10;
	
	UPROPERTY(EditdefaultsOnly, Category = "PoolingSettings")
	int32 BackUpPoolSize = 5;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDPARTITIONHELPER_API UWPHelperPoolingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWPHelperPoolingComponent();

	virtual AController* RequestController(APawn* inInstigator, const TSubclassOf<AController>& inControllerClass);
	virtual void ReleaseController(AController* inController,const TSubclassOf<AController>& inControllerClass);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditdefaultsOnly, Category = "PoolingSettings")
	TMap<TSubclassOf<AController>,FAIControllerPoolSettings> AIControllerPoolSettings;

private:

	UPROPERTY()
	TMap<TSubclassOf<AController>,FAiControllerQueue> AIControllerQueue;

	void InitializePool();
	void RequestBackUpElements(const TSubclassOf<AController>& inControllerClass);

};
