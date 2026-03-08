#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "NebulaFlowSaveFailedTask.generated.h"

class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowSaveFailedTask : public UNebulaFlowTaskInterface
{
GENERATED_BODY()

public:

	void InitializeTask();

protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;
	void SuspensionImpementation() override;
	
	void OnTermination() override;

	UFUNCTION()
	void OnDialogResponseLoop(const FString& inResponse);

	UFUNCTION()
	void OnDialogResponseContinueWithouSaving(const FString& inResponse);


	void ContinueExecutionImplementation() override;

private:

	void ShowDialogLoop();
	void ShowDialogContinueWithouSaving();
	void HandleState();

	UPROPERTY()
	UNebulaFlowDialog* Dialog = nullptr;

	UPROPERTY(EditAnywhere)
		FName DialogIdContinueWithouSaving;

	UPROPERTY(EditAnywhere)
		FName DialogIdContinueWithouSavingPS4;

	UPROPERTY(EditAnywhere)
		FName DialogIdContinueWithouSavingPS5;

	UPROPERTY(EditAnywhere)
	FName DialogIdLoop;

	bool bHandled = false;
};