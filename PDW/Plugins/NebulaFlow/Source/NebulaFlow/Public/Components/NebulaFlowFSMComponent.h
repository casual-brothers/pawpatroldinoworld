// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "NebulaFlowFSMComponent.generated.h"

class UNebulaFlowBaseFSM;
class UNebulaFlowFSMManager;

UCLASS(BlueprintType,Blueprintable, meta=(BlueprintSpawnableComponent))
class NEBULAFLOW_API UNebulaFlowFSMComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	TSubclassOf<UNebulaFlowBaseFSM> FSMClass;

private:

	UPROPERTY()
	UNebulaFlowBaseFSM* FSMRef;

	UPROPERTY()
	UNebulaFlowFSMManager* FSMManagerRef;

};