// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/Actor/FlowNode_OnNotifyFromActor.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFlowNode_OnNotifyFromActor.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWFlowNode_OnNotifyFromActor : public UFlowNode_OnNotifyFromActor
{
	GENERATED_BODY()
	
public:

	UPDWFlowNode_OnNotifyFromActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere)
	FPDWQuestTargetData TargetsData {};

	UPROPERTY(EditAnywhere)
	bool InitWithInputPins = false;

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<UDataLayerAsset>, bool> OnEnterDataLayersInfo {};

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<UDataLayerAsset>, bool> OnExitDataLayersInfo {};

protected:

	void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	void ExecuteInput(const FName& PinName) override;

	void SetTargets();

	void OnLoad_Implementation() override;

	void UpdateDataLayer(TMap<TObjectPtr<UDataLayerAsset>, bool> DLInfo);

	void TriggerOutput(FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;


	void OnPassThrough_Implementation() override;

};
