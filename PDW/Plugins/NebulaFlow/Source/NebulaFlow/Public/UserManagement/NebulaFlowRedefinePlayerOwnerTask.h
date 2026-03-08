#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "Misc/Optional.h"
#include "OnlineError.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "NebulaFlowRedefinePlayerOwnerTask.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowRedefinePlayerOwnerTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask(int32 userIndex);

protected:

	void ExecutionImplementation() override;
	bool IsValidImplementation() override;

private:

	void HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& Error);
	void OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	void SendToWelcomePage();

private:

	TOptional<int32> PendingControllerIndex;
	int32 UserIndex;
};