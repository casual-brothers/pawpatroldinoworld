#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "Online/CoreOnline.h"
#include "OnlineError.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/Optional.h"
#include "NebulaFlowDefineSecondaryPlayerTask.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowDefineSecondaryPlayerTask : public UNebulaFlowTaskInterface
{
	GENERATED_BODY()

public:

	void InitializeTask(int32 iUserIndex);

protected:

	bool IsValidImplementation() override;
	void ExecutionImplementation() override;

private:

	void HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& Error);
	void OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

private:

	TOptional<int32> PendingControllerIndex;
	int32 UserIndex;
};