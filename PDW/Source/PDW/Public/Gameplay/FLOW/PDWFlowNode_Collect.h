// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_Collect.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Collect"))
class PDW_API UPDWFlowNode_Collect : public UFlowNode
{
	GENERATED_BODY()
	
public:

	UPDWFlowNode_Collect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!InitWithInputPins", EditConditionHides))
	FGameplayTag CollectibleTag;

	UPROPERTY(EditAnywhere)//, meta = (EditCondition = "!InitWithInputPins", EditConditionHides))
	int32 QuantityToCollect = 1;

	UPROPERTY(EditAnywhere)
	bool InitWithInputPins = false;

	UPROPERTY(EditAnywhere)
	bool bUseLastOutputPin = false;

	void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


protected:
	void OnLoad_Implementation() override;

	UFUNCTION()
	void OnItemCollected(FGameplayTag ItemTag);

	UFUNCTION()
	void BindCollectEvent();

	UFUNCTION()
	void UnbindCollectEvent();


	UPROPERTY(VisibleAnywhere, SaveGame)
	int32 QuantityCollected = 0;

	//UPROPERTY(EditAnywhere, Category = "Notify", meta = (EditCondition = "InitWithInputPins", EditConditionHides, DefaultForInputFlowPin, FlowPinType = GameplayTag))
	//FGameplayTag InputCollectibleTag;



	void TriggerOutput(FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;

};
