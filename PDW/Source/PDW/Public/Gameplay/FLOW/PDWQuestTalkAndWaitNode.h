// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/FLOW/Dialogue/PDWTalkAndWaitNode.h"
#include "PDWQuestTalkAndWaitNode.generated.h"

class UFlowComponent;

UCLASS(NotBlueprintable, meta = (DisplayName = "Questgiver Talk and Wait"))
class PDW_API UPDWQuestTalkAndWaitNode : public UPDWTalkAndWaitNode
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer ActorTarget = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGameplayContainerMatchType MatchType = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSearchExactTagMatch = true;

	virtual void ToggleQuestGiver(bool IsActive);

	void OnLoad_Implementation() override;

	void TriggerFirstOutput(const bool bFinish) override;

	void OnPassThrough_Implementation() override;

	void TriggerOutput(FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;

	UFUNCTION()
	virtual void OnComponentRegistered(UFlowComponent* Component);

public:

	void ExecuteInput(const FName& PinName) override;

private:

	UPROPERTY()
	bool CurrentIsActive = false;

	UFUNCTION()
	void HandleQuestGiverTag(AActor* TagOwner);

};
