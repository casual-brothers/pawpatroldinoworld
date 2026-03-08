// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "PDWMinigameTargetComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("UPDWMinigameTargetComponent"), STATGROUP_MinigameTargetComp, STATCAT_Advanced);

class UPDWMinigameConfigComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWMinigameTargetComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWMinigameTargetComponent();

	void RegisterMinigameOwner(UPDWMinigameConfigComponent* inOwner);

	UFUNCTION(BlueprintCallable)
	virtual void ResetActor();
	UFUNCTION(BlueprintCallable)
	virtual void SaveStartingState();
	UFUNCTION(BlueprintCallable)
	virtual void OnMinigameStarted();
	UFUNCTION(BlueprintCallable)
	virtual void OnMinigameEnded();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMinigameStarted();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMinigameEnded();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bShouldBeDestroyedOnEnd = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UNiagaraSystem* InteractionVFX = nullptr;

	UPROPERTY()
	UPDWMinigameConfigComponent* MinigameOwner = nullptr;
	UPROPERTY()
	UNiagaraComponent* InteractionVFXComponent = nullptr;
private:

};
