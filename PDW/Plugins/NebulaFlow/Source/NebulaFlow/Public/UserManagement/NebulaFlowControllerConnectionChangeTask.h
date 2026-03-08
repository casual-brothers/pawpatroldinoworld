#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h"
#include "NebulaFlowControllerConnectionChangeTask.generated.h"

class UNebulaFlowDialog;
class UTexture2D;

UCLASS()
class NEBULAFLOW_API UNebulaFlowControllerConnectionChangeTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask(bool bIsConnected, FPlatformUserId UserId, int32 inputDeviceId);

protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;
	void SuspensionImpementation() override;
	void OnTermination() override;


private:

	void ShowDialogOnPause();
	void ShowDialogWithoutPause();
	void OnDialogResponse(const FString& inResponse);
	bool CanPause();

private:

	UPROPERTY()
	UNebulaFlowDialog* Dialog = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> PSIcon;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> WinIcon;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> KeyboardIcon;

	UPROPERTY(EditAnywhere)
	FName DialogId;

	UPROPERTY(EditAnywhere)
	bool IsFirstPlayer = true;

	UPROPERTY(EditAnywhere)
	bool bShowXboxSystemDialog = true;

	UPROPERTY(EditAnywhere)
	bool PauseOnDisconnect = true;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!PauseOnDisconnect"))
	float DialogDuration;

	UPROPERTY()
	FTimerHandle TimerHandler;

	bool IsConnected;
	FPlatformUserId UserId;
	int32 InputDeviceId;

	bool bDoneOnce = false;
};