// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "../Classes/FMODEvent.h"
#include "PDWFlowNode_Conversation.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Trigger Conversation"))
class PDW_API UPDWFlowNode_Conversation : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere, Category = "Config", meta=(EditCondition="bHideConversationProperty == false", EditConditionHide))
	FConversation Conversation;

	UPROPERTY(VisibleAnywhere, Category = "Config")
	bool bHideConversationProperty = false;

	void OnLoad_Implementation() override;

	void TriggerConversation();

public:
	void Cleanup() override;

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Utility")
	void PlayVoiceOver();
#endif

#if WITH_EDITORONLY_DATA	
	UPROPERTY(EditAnywhere, Category = "Utility")
	int32 VoiceOverIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Utility")
	UFMODEvent* VoiceOverEvent = nullptr;
#endif
};
