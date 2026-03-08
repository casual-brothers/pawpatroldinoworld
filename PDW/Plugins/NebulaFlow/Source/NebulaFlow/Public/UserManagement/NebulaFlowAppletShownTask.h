// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "NebulaFlowAppletShownTask.generated.h"

/**
 * 
 */
UCLASS()
class NEBULAFLOW_API UNebulaFlowAppletShownTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()
	

public:

	void InitializeTask();
	bool IsValidImplementation() override;
	void OnTermination() override;

protected:

	UPROPERTY()
	UGameInstance* GameInstance;

	void ExecutionImplementation() override;
	virtual void ContinueExecutionImplementation() override;

	void OnUIClosed(bool success);

	bool bTerminateTask = false;
};
