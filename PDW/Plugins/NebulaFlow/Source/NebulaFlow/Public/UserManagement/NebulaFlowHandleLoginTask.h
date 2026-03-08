#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "OnlineSubsystemTypes.h"
#include "NebulaFlowHandleLoginTask.generated.h"

class UNebulaFlowDialog;
class ULocalPlayer;

UCLASS()
class NEBULAFLOW_API UNebulaFlowHandleLoginTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask(int32 gameUserIndex, ELoginStatus::Type previousLoginStatus, ELoginStatus::Type loginStatus, const FUniqueNetId& userId);
	
protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;
	void OnTermination() override;

	void OnDialogResponse(const FString& inResponse);

	UFUNCTION()
	void HandleUserLoginChangedDelayed();

	UFUNCTION()
	void OnTimerExpired();

	UFUNCTION()
	void SendToWelcomePage();

protected:

	UPROPERTY()
	UNebulaFlowDialog* Dialog = nullptr;

	FText ErrorText;

	FTimerHandle TimerHandler;

	UPROPERTY(EditAnywhere)
	FName UserLoginDialogId;

	UPROPERTY(EditAnywhere)
	FName WinGDKDialogId;

	UPROPERTY(EditAnywhere)
	bool ForcePause = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!ForcePause"))
	float PopupTime = 5.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ForcePause"))
	FName ReturnFsmLabel;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ForcePause"))
	FString SendToWelcomeAction = TEXT("Yes");

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ForcePause"))
	FString ProfileSwapAction = TEXT("ProfileSwap");

private:

	int32 GameUserIndex;
	ELoginStatus::Type PreviousLoginStatus;
	ELoginStatus::Type LoginStatus;

	UPROPERTY()
	ULocalPlayer* LocalPlayer = nullptr;

	float UserLoginChangedDelay = 1.0f;

	static const FName FROM_PROFILE_CHANGED_LABEL;

	bool bShouldPause = false;

	bool bDowngraded;
};