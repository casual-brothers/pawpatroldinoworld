// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FSM/NebulaFlowBaseFSMState.h"
#include "GameplayTagContainer.h"
#include "PDWGameplayFSMstate.generated.h"

class APlayerController;

/**
 * 
 */
UCLASS()
class PDW_API UPDWGameplayFSMstate : public UNebulaFlowBaseFSMState
{
	GENERATED_BODY()
public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

	void OnFSMStateExit_Implementation() override;

private:

	void HandleSkillsOnCHangeState(APlayerController* inController = nullptr);
	void RemoveGameplayEffectOnUIInteraction(APlayerController* inController = nullptr);

	UPROPERTY()
	TMap<APlayerController*, bool> IsClosingCharacterSelectorByPlayer;
};
