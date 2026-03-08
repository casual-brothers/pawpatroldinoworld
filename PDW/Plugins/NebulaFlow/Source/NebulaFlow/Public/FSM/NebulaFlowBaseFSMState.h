// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Managers/NebulaFlowAudioManager.h"
#include "StateBehaviors/NebulaFlowBaseStateBehavior.h"
#include "NebulaFlowBaseFSMState.generated.h"


class UNebulaFlowBaseFSM;
class UNebulaFlowAudioManager;
class UNebulaFlowBaseStateInitCondition;

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class NEBULAFLOW_API UNebulaFlowBaseFSMState : public UObject
{
	GENERATED_UCLASS_BODY()

	friend class UNebulaFlowBaseFSM;
	friend class UNebulaFlowFSMManager;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
	void OnFSMStateEnter(const FString& InOption = FString(""));
	
	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
	void OnFSMStateExit();

	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
	void OnFSMStateUpdate(float deltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "FSM")
	void OnFSMStateAction(const FString& Action, const FString& Parameter, APlayerController* ControllerSender);

	UFUNCTION(BlueprintCallable, Category="FSM")
	void TriggerTransition(const FName TransitionName, const FName LabelName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "FSM")
	void TriggerTransitionWithOption(const FName TransitionName, FString InOption, const FName LabelName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "FSM")
	void TriggerLabelTransition(const FName LabelName, FString InOption = FString(""));

	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "FSM")
	const UNebulaFlowBaseFSM* GetOwnerFSM(){return OwnerFSM;}

	bool HasInitConditions();

	bool HasPendingConditions();

protected:

	virtual void InitState(FName InStateName,UNebulaFlowBaseFSM* InOwner);

	virtual void UnInitState();

	FName StateParameter = NAME_None;

	UPROPERTY(EditDefaultsOnly,Category = "FSM|Audio");
	TArray<FFsmAudioOperation> AudioOperations{};

	UPROPERTY(EditAnywhere,Instanced,Category = "FSM|StateBehaviors")
	TArray<UNebulaFlowBaseStateBehavior*> StateBehaviors{};

	UPROPERTY(EditAnywhere, Category = "FSM|InitializationConditions")
	TArray<TSubclassOf<UNebulaFlowBaseStateInitCondition>> InitConditionsClasses;

	FDelegateHandle OnActionDelegateHandler;

	FName StateName;

	UPROPERTY()
	UNebulaFlowBaseFSM* OwnerFSM;

private:

	UPROPERTY()
	UNebulaFlowAudioManager* AudioManagerRef = nullptr;

	bool bIsExiting = false;

	UPROPERTY()
	TArray<UNebulaFlowBaseStateInitCondition*> _InternalConditionsObjects{};
};