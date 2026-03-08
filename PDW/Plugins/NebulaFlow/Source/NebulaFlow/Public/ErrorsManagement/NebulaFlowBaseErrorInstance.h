// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Managers/NebulaFlowErrorManager.h"
#include "NebulaFlowBaseErrorInstance.generated.h"

class UNebulaFlowLocalPlayer;
class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowBaseErrorInstance : public UObject
{

	GENERATED_BODY()

	friend class UNebulaFlowErrorManager;

public:

	UNebulaFlowBaseErrorInstance(const FObjectInitializer& ObjectInitializer);

	virtual bool IsErrorConditionValid();

	void SetErrorText(FText inErrorText);

	virtual UWorld* GetWorld() const override;

protected:

	//Error Instance Configuration
	
	bool bShouldTick = true;

	EErrorInstanceType ErrorInstanceType = EErrorInstanceType::EDefault;
	
	FName SystemDialogID = NAME_None;

	FText ErrorText{};

	bool bErrorConditionValid = true;

	//End Configuration

	UPROPERTY()
	UNebulaFlowErrorManager* ErrorManagerRef = nullptr;

	UPROPERTY()
	UNebulaFlowLocalPlayer* PlayerOwnerRef = nullptr;

	UPROPERTY()
	UNebulaFlowDialog* SystemDialog = nullptr;

	virtual void TickErrorInstance(float DeltaTime);

	virtual void InitErrorInstance(UNebulaFlowErrorManager* InErrorManager,UNebulaFlowLocalPlayer* InLocalPlayer, FName inDialogID = NAME_None);

	virtual void UninitErrorInstance();

	virtual void StartManagingError();


};