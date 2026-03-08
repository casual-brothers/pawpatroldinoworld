// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ErrorsManagement/NebulaFlowBaseErrorInstance.h"
#include "NebulaFlowDefaultErrorInstance.generated.h"

class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowDefaultErrorInstance : public UNebulaFlowBaseErrorInstance
{

	GENERATED_BODY()

public:

	UNebulaFlowDefaultErrorInstance(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void StartManagingError() override;

private:

	static const FName DEFAULT_SYSTEM_DIALOG_ID;

	

	void OnDialogResponse(FString Response);

};