// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "Gameplay/MiniGames/PDWButtonSequenceInputBehaviour.h"
#include "EnhancedInputComponent.h"
#include "PDWGrapplingHookShootBehaviour.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FGrapplingHookConfiguration : public FInputActionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	TArray<FButtonSequenceConfiguration> ButtonSequenceConfig;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	FGameplayTagContainer ActionToUnbind;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	bool bShuffleSequence = false;
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWGrapplingHookShootBehaviour : public UPDWMiniGameInputBehaviour
{
	GENERATED_BODY()
	
public:
	
	UWorld* GetWorld() const override;


	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;

protected:

	UPROPERTY(BlueprintReadWrite)
	bool bIsValidTarget = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> CurrentTarget;

	UFUNCTION()
	void OnTargetAquired(const TArray<AActor*>& Target,APDWPlayerController* ControllerSender);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTargetAquired(const TArray<AActor*>& Target,APDWPlayerController* ControllerSender);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnComplete();

	UFUNCTION(BlueprintCallable)
	void UpdateBind();

	UFUNCTION(BlueprintCallable)
	void NotifySuccess();

	UFUNCTION(BlueprintCallable)
	void ShuffleButtonSequence();
private:

	TArray<int32> BindingHandleToRemove;
	UPROPERTY()
	TArray<FButtonSequenceConfiguration> ShuffledArray;
	int32 CurrentIndex = 0;
};
