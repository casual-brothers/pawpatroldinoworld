// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDWFlowNode_OnNotifyFromActor.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "PDWFlowNode_OnInteractionFinish.generated.h"

UENUM(BlueprintType)
enum class ENotifyType : uint8
{
	BehaviourSuccess = 0,
	StateChanged = 1
};

UCLASS(NotBlueprintable, meta = (DisplayName = "Wait for interaction"))
class PDW_API UPDWFlowNode_OnInteractionFinish : public UPDWFlowNode_OnNotifyFromActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataLayerAsset> DataLayerToReset {};

protected:

	UPROPERTY(EditAnywhere)
	ENotifyType NotifyType = ENotifyType::BehaviourSuccess;

	UPROPERTY(EditAnywhere)
	bool KeepSavedState = false;

	void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;


	void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	UFUNCTION()
	void OnInteractionSuccessful(const FPDWInteractionPayload& Payload);

	UFUNCTION()
	void OnStateTagAdded(UPDWInteractionReceiverComponent* Component, const FGameplayTag& ChangedTag, const FGameplayTag& Tag);

	void TriggerOutput(FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;


	void OnPassThrough_Implementation() override;


	void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	void BindEvents();

	UFUNCTION()
	void UnbindEvents();


	void OnLoad_Implementation() override;

	UFUNCTION()
	void ResetDl();

private:

	UPROPERTY()
	bool ResettingDL = false;

    FTimerHandle ResetDLTimerHandler;

};
