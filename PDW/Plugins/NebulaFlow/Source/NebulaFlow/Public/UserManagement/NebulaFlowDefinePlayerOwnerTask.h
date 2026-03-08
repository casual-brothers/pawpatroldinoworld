#pragma once

#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "Misc/Optional.h"
#include "OnlineError.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "NebulaFlowDefinePlayerOwnerTask.generated.h"

class UNebulaFlowDialog;

UCLASS()
class NEBULAFLOW_API UNebulaFlowDefinePlayerOwnerTask : public UNebulaFlowTaskInterface
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

protected:

	UPROPERTY()
	UNebulaFlowDialog* Dialog = nullptr;

	UPROPERTY(EditAnywhere)
	FName UnlicensedDialogId;

	UPROPERTY(EditAnywhere)
	FName ProgressWillNotBeSavedDialogId;

private:

	TOptional<int32> PendingControllerIndex;
	int32 UserIndex;
};