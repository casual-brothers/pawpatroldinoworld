// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "NebulaFlowBaseFSM.generated.h"

class UNebulaFlowBaseFSMState;
class UNebulaFlowGameInstance;

UENUM(BlueprintType)
enum class EFsmMode : uint8
{
	AsyncMode,
	SyncMode  //Experimental
};

UENUM(BlueprintType)
enum class EStatePhase : uint8
{
	EBeforeOnEnter,
	EOnEnter,
	EOnExit
};

USTRUCT()
struct FFSMStateDescriptor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	FName StateParameter = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	TSubclassOf<UNebulaFlowBaseFSMState> FSMStateClass{};

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	TMap<FName, FName> TransitionToStateMap{};
};

UCLASS(BlueprintType,Blueprintable)
class NEBULAFLOW_API UNebulaFlowBaseFSM : public UObject
{
	GENERATED_UCLASS_BODY()

	friend class UNebulaFlowFSMManager;

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	EFsmMode FSMMode = EFsmMode::AsyncMode;
	
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bEditable"), Category = "FSM")
	FName DefaultLabelName = FName("Default");

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bEditable"), Category = "FSM")
	TMap<FName, FName> LabelToEntryPointMap{};

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bEditable"), Category = "FSM")
	TMap<FName, FFSMStateDescriptor> FSMStructureMap{};

	virtual void InitFSM();

	virtual void UninitFSM();

	virtual void UpdateFSM(float deltaTime);

	virtual void SetCanUpdateFSM(bool bInCanUpdate);

	void TriggerLabel(FName LabelName);

	bool IsInBetweenTransition();

	bool TriggerTransition(const FName TransitionName, FName LabelName = NAME_None);

	bool TriggerTransitionWithOption(const FName TransitionName, FString InOption, FName LabelName = NAME_None);

	bool TriggerLabelTransition(const FName LabelName, FString InOption = FString(""));

	virtual UWorld* GetWorld() const override;

	void GoToEntryPoint();

protected:	

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	bool bEditable = false;

	UFUNCTION()
	void OnLoadLevelStarted(FName LevelName);

	UFUNCTION()
	void OnLoadLevelEnded(FName LevelName);

	virtual FName GetEntryPointStateLabel();

private:
	
	FString PendingOption = "";

	FName PendingTransition = NAME_None;
	
	bool bCanUpdate = true;

	bool bHasBeenInitialized = false;

	bool bIsBetweenTransition = false;
	
	FDelegateHandle LevelLoadStartHandle;

	FDelegateHandle LevelLoadEndedHandle;

	void ManageTransition();

	void ManageBehaviors(UNebulaFlowBaseFSMState* CurrentState, EStatePhase CurrentPhase);

	void ResolveInitConditions(float deltaTime);

	bool bIsWaitingInitConditions = false;

protected:

	UPROPERTY()
	UNebulaFlowBaseFSMState* CurrentFsmState = nullptr;

	virtual void BeginCurrentState();

	void EndCurrentState();
};