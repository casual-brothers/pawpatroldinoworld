// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWMinigameSucceedFSMState.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigameSucceedFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()

public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EQuestMessageType MessageType = EQuestMessageType::MinigameCompleted;
};
