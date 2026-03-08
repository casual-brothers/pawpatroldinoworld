// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "NebulaFlowRemapJoyconsTask.generated.h"

class UNebulaFlowUserSubSystem;
class UNebulaFlowDialog;
/**
 * 
 */
UCLASS()
class NEBULAFLOW_API UNebulaFlowRemapJoyconsTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()
	
public:

	void InitializeTask(int CurrentInGamePlayers);
	bool IsValidImplementation() override;
	void OnTermination() override;

protected:

	void ExecutionImplementation() override;
	virtual void ContinueExecutionImplementation() override;

	UPROPERTY()
	UNebulaFlowUserSubSystem* SubSystemRef {};

	void OnConnectedJoyconsUpdate(const int JoyconsConnected);

	FName MissingControllersID = FName("NOT_ENOUGH_CONTROLLERS");

	FName AreYouSureID = FName("ARE_YOU_SURE");

	int JoyconsConnected = 0;
	int InGamePlayers = 0;

	float FrameToWaitBeforeDialog = 3;

	volatile bool TerminateTask = false;

	UPROPERTY(BlueprintReadWrite)
		UNebulaFlowDialog* RemapDialogOnScreen {};

	UPROPERTY(BlueprintReadWrite)
		UNebulaFlowDialog* ReturnToMainMenuDialogOnScreen {};

	void OnSwitchDialogResponse(FString Response);

	void CloseConfirmationResponse(FString Response);
};
