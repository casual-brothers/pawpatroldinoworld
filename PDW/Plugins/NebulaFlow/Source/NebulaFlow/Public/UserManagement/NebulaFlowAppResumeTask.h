#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "NebulaFlowAppResumeTask.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowAppResumeTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask();

protected:
	
	void ExecutionImplementation() override;
	bool IsValidImplementation() override;

private:

	void ShowDialog(bool bIsFirstPlayer);
	void OnDialogResponse(const FString& inResponse);

protected:

	UPROPERTY(EditAnywhere)
	FName DialogId;

	UPROPERTY(EditAnywhere)
	FName SecondPlayerDisconnectDialogId;

	UPROPERTY(EditAnywhere)
	FName ReturnFsmLabel;

	UPROPERTY()
	class UNebulaFlowDialog* SystemDialog;

};