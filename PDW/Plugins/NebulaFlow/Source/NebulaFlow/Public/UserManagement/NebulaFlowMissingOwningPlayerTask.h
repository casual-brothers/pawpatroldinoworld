#pragma once

#include "NebulaFlowControllerPairingTask.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "Misc/CoreMiscDefines.h"
#include "NebulaFlowMissingOwningPlayerTask.generated.h"

class UNebulaFlowDialog;
class UNebulaFlowLocalPlayer;

UCLASS()
class NEBULAFLOW_API UNebulaFlowMissingOwningPlayerTask : public UNebulaFlowControllerPairingTask
{
	GENERATED_BODY()

public:

	void InitializeTask(int32 localUserNum, const FControllerPairingChangedUserInfo& previousUser, const FControllerPairingChangedUserInfo& newUser);

protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;
	void OnTermination() override;

private:

	void ShowDialog();

	UFUNCTION()
	void SendToWelcomePage();

	UFUNCTION()
	void OnDialogChoice(const FString& inResponse);

private:

	UPROPERTY()
	UNebulaFlowDialog* SystemDialog = nullptr;

	UPROPERTY(EditAnywhere)
	FName DialogId;

	UPROPERTY(EditAnywhere)
	FString SendToWelcomeAction = TEXT("Yes");

	UPROPERTY(EditAnywhere)
	FString ProfileSwapAction = TEXT("ProfileSwap");
};