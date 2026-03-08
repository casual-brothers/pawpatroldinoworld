// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "GameplayTagContainer.h"
#include "PDWDinoCustomizationUIFSMState.generated.h"

class UPDWCustomizationPage;

/**
 * 
 */
UCLASS()
class PDW_API UPDWDinoCustomizationUIFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
private:

	bool bCurrentlyInTransition = false;
protected:

	UPROPERTY()
	UPDWCustomizationPage* CustomizationPage;
	
	UPROPERTY()
	FGameplayTag DinoTagToCustomize;

	UPROPERTY()
	FGameplayTag StartingCustomization = {};

	UPROPERTY()
	FGameplayTag CurrentCustomization = {};

	UFUNCTION()
	void OnTransitionInEnd();

	UFUNCTION()
	void OnFirstTeleportComplete();

	UFUNCTION()
	void CustomizeDino(FGameplayTag NewCustomization);

	UFUNCTION()
	void ResetToStartingCustomization();

	UPROPERTY()
	bool CurrentPenActivity = false;

public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;
	void OnFSMStateExit_Implementation() override;
};
