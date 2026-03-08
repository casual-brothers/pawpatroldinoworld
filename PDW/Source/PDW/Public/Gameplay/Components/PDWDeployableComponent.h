// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDWMinigameTargetComponent.h"
#include "PDWDeployableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMiniGameStarted);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWDeployableComponent : public UPDWMinigameTargetComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWDeployableComponent();

	void OnMinigameEnded() override;
	void OnMinigameStarted() override;

	UFUNCTION(BlueprintCallable)
	void OnObjectDetached();
	UFUNCTION(BlueprintCallable)
	void OnObjectAttached();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnObjectDetached();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnObjectAttached();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UNiagaraSystem* AttachDetachVFX = nullptr;

	FTransform InitialTransform = FTransform();

	FGameplayTag InitialState = FGameplayTag::EmptyTag;
public:
	void ResetActor() override;

	UPROPERTY(BlueprintAssignable)
	FOnMiniGameStarted OnMinigameStartedEvent;
};
