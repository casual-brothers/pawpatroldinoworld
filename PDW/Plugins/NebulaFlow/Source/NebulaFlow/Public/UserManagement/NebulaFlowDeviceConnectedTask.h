#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "Core/NebulaFlowGameInstance.h"
#include "NebulaFlowDeviceConnectedTask.generated.h"

class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowDeviceConnectedTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask(void* iDevice);

protected:

	bool IsValidImplementation() override;
	void ExecutionImplementation() override;
	void SuspensionImpementation() override;
	void OnTermination() override;

	UFUNCTION()
	void OnTimerExpired();

	UFUNCTION()
	void DelayedExecution();

protected:

	UPROPERTY(EditAnywhere)
	FName PopupId;

	UPROPERTY(EditAnywhere)
	bool bShowPopup = false;

	UPROPERTY(EditAnywhere)
	float PopupTime;

	UPROPERTY(EditAnywhere)
	TArray<EGameContext> AvailableContexts;

	void* Device;

	bool bIsReady = false;

	UPROPERTY()
	UNebulaFlowDialog* Popup;

	FTimerHandle PopupTimerHandler;
	FTimerHandle DelayTimerHandler;
};