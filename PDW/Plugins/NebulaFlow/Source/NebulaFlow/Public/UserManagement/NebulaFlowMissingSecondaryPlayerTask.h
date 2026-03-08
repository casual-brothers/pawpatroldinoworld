#pragma once

#include "NebulaFlowControllerPairingTask.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "NebulaFlowMissingSecondaryPlayerTask.generated.h"

class UNebulaFlowDialog;
class UNebulaFlowLocalPlayer;

UCLASS()
class NEBULAFLOW_API UNebulaFlowMissingSecondaryPlayerTask : public UNebulaFlowControllerPairingTask
{
	GENERATED_BODY()

public:

	void InitializeTask(int32 localUserNum, const FControllerPairingChangedUserInfo& previousUser, const FControllerPairingChangedUserInfo& newUser);

protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;
	void OnTermination() override;

	UFUNCTION()
	void OnTimerExpired();

private:

	void ShowDialog();

private:

	UPROPERTY()
	UNebulaFlowDialog* PopupDialog = nullptr;

	UPROPERTY(EditAnywhere)
	FName DialogId;

	UPROPERTY(EditAnywhere)
	float PopupTime;

	FTimerHandle PopupTimerHandler;
};