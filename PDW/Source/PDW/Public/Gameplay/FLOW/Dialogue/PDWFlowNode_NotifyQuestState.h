// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/PDWGameplayStructures.h"
#include "Gameplay/FLOW/PDWFlowNode_ChangeState.h"
#include "PDWFlowNode_NotifyQuestState.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Notify Quest State"))
class PDW_API UPDWFlowNode_NotifyQuestState : public UPDWFlowNode_ChangeState
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

protected:
	virtual void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	void HandleQuestNotify();

	UPROPERTY(EditAnywhere)
	EQuestMessageType QuestMessageType = EQuestMessageType::StartQuest;

	UPROPERTY(EditAnywhere)
	bool bShowMessage = true;

	void ChangeState() override;


	void OnPassThrough_Implementation() override;


	void OnLoad_Implementation() override;

};
