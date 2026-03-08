// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "Conditions/NebulaInteractionConditionCheck.h"
#include "Behaviours/NebulaInteractionBehaviour.h"
#include "Data/NebulaInteractionSystemStructs.h"
#include "InteractionExecutionNode.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "ExecuteInteraction", Keywords = "ExecuteInteraction"))
class NEBULAINTERACTIONSYSTEM_API UInteractionExecutionNode : public UFlowNode
{
	GENERATED_BODY()

	UInteractionExecutionNode(const FObjectInitializer& ObjectInitializer);

public:

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif

	UPROPERTY(EditAnywhere)
	FFlowDataPinInputProperty_InstancedStruct InteractionInfoPin;

	UPROPERTY(EditAnywhere,Instanced, Category = "Configuration")
	TArray<UNebulaInteractionConditionCheck*> InteractionConditions;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	UNebulaInteractionBehaviour* BehaviourToExecute = nullptr;

	UPROPERTY(EditAnywhere,Category = "Configuration", meta =(Tooltip ="If true, this behaviour can be triggered at the same time from multiple interactor"))
	bool bAcceptMultipleConcurrentInteractor = false;

	void ExecuteInput(const FName& PinName) override;
	void Cleanup() override;

protected:

	static FName INPIN_InteractionInfoInstancedStruct;
	static FName OUTPIN_FailedCondition;
	static FName OUTPIN_InteractionSuccess;
	static FName OUTPIN_InteractionInterrupted;

	UFUNCTION()
	void OnInteractionFinished(const FInteractionBehaviourEventInfo& inEventInfo);
	void OnInteractionInterrupted(const FInteractionBehaviourInterruptInfo& inEventInfo);

private:
	
	UPROPERTY()
	TSet<AActor*> Interactors = {};

	FInstancedStruct  ResolveInstancedStruct() const;
	bool EvaluateCondition(const FInteractionInfo& inInteractionInfoSIgnature);
	bool InitializeInteractionBehaviour(const FInteractionInfo& inInteractionInfo) const;
	bool CanBeInteracted(AActor* inInteractor);
	void Unbind();

};
