// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ErrorsManagement/NebulaFlowBaseErrorInstance.h"
#include "NebulaFlowProfileChangedErrorInstance.generated.h"

class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowProfileChangedErrorInstance : public UNebulaFlowBaseErrorInstance
{

	GENERATED_BODY()

public:

	UNebulaFlowProfileChangedErrorInstance(const FObjectInitializer& ObjectInitializer);

	static const FName PROFILE_CHANGED_DIALOG_ID;
	static const FName FROM_PROFILE_CHANGED_LABEL;
protected:

	virtual void StartManagingError() override;

private:

	void OnDialogResponse(FString Response);

};