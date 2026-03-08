// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "UI/Widgets/PDWTutorialNavButton.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "PDWTriggerTutorialNavButton.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Wait for Tutorial Input Trigger"))
class PDW_API UPDWTriggerTutorialNavButton : public UFlowNode
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPDWTutorialButtonData ButtonData {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FConversation ConversationToPlay {};

	UFUNCTION()
	void OnIputTrigger();

	void OnLoad_Implementation() override;

	UFUNCTION()
	void ExecuteNode();

	UFUNCTION()
	void OnHUDReady();

public:
	UPDWTriggerTutorialNavButton(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ExecuteInput(const FName& PinName) override;
};
