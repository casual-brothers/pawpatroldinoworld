// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Flow/Nodes/FlowNode_OnTriggerEnter.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFlowNode_OnTriggerEnter.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Wait for Trigger Enter"))
class PDW_API UPDWFlowNode_OnTriggerEnter : public UFlowNode_OnTriggerEnter
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	FPDWQuestTargetData TargetsData {};

protected:

	void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	void ExecuteInput(const FName& PinName) override;

	void OnTriggerEvent(const bool bOverlapping, UFlowComponent* OtherFlowComponent, UFlowComponent* SourceFlowComponent) override;

	void SetTargets();

	void OnLoad_Implementation() override;

};
