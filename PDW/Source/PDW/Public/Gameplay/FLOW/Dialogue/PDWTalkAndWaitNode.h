// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "../PDWFlowNode_ChangeState.h"
#include "PDWTalkAndWaitNode.generated.h"

class UPDWDialogueFlowAsset;

UCLASS(NotBlueprintable, meta = (DisplayName = "Talk and Wait", Keywords = "Talk"))
class PDW_API UPDWTalkAndWaitNode : public UPDWFlowNode_ChangeState
{
	GENERATED_BODY()
	
public:

	UPDWTalkAndWaitNode();

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FGameplayTag DialogueID = FGameplayTag::EmptyTag;

	void ExecuteInput(const FName& PinName) override;

	UPROPERTY()
	bool bIsTriggered = false;

	UFUNCTION()
	void OnQuestTalking();

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
protected:
	void ContinueToNextNode() override;


	void OnLoad_Implementation() override;

	void SetDialogueEntryPoint();

};
