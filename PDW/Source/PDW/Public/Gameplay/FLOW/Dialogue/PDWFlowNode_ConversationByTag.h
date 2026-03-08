// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "PDWFlowNode_Conversation.h"
#include "PDWFlowNode_ConversationByTag.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FConversationByTagDataEntry
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TagsContainer{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConversation Conversation{};
};

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Trigger Conversation By Tag"))
class PDW_API UPDWFlowNode_ConversationByTag : public UPDWFlowNode_Conversation
{
	GENERATED_BODY()
	
	UPDWFlowNode_ConversationByTag(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void ExecuteInput(const FName& PinName) override;

	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<FConversationByTagDataEntry> Conversations;
};
