#pragma once

#include "NebulaFlowControllerPairingTask.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "NebulaFlowMultipleControllerTask.generated.h"

class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowMultipleControllerTask : public UNebulaFlowControllerPairingTask
{
	GENERATED_BODY()

protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;

private:
	
	void ShowDialog();

protected:

	UPROPERTY(EditAnywhere)
	FName DialogId;

	UPROPERTY(EditAnywhere)
	float DialogDuration = 5.f;

	UPROPERTY()
	UNebulaFlowDialog* Dialog = nullptr;

	FTimerHandle TimeHandler;

};