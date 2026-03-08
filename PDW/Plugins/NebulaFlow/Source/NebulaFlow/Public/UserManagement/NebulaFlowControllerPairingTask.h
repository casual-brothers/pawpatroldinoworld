#pragma once

#include "Interfaces/OnlineIdentityInterface.h"
#include "UObject/Object.h"
#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "NebulaFlowControllerPairingTask.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowControllerPairingTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask(int32 localUserNum, const FControllerPairingChangedUserInfo& previousUser, const FControllerPairingChangedUserInfo& newUser);

protected:

	virtual bool IsValidImplementation(){return false;};
	int32 GetControllerIdFromControllerPairingChangedUserInfo(const FControllerPairingChangedUserInfo& user) const;
	FPlatformUserId GetPlatformUserIdFromControllerPairingChangedUserInfo(const FControllerPairingChangedUserInfo& user) const;

protected:

	int32 LocalUserNum;
	int32 PreviousUserId;
	int32 NextUserId;
	FPlatformUserId PreviousPlatformUserId;
	FPlatformUserId NextPlatformUserId;
};