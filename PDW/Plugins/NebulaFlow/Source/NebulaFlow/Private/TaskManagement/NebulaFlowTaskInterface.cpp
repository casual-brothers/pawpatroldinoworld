#include "TaskManagement/NebulaFlowTaskInterface.h"

void UNebulaFlowTaskInterface::SetPriority(int32 priority)
{
	Priority = priority;
}

int32 UNebulaFlowTaskInterface::GetPriority() const
{
	return Priority;
}

ETaskStatus UNebulaFlowTaskInterface::GetStatus() const
{
	return Status;
}

bool UNebulaFlowTaskInterface::IsValid()
{
	bool bIsValid = IsValidImplementation();
	if (!bIsValid)
	{
		Terminate();
		//OnTermination();
	}
	return bIsValid;
}

void UNebulaFlowTaskInterface::Execute()
{
	switch (Status)
	{
	case ETaskStatus::ECreated:
		HandleExecution();
		break;
	case ETaskStatus::EExecute:
		ContinueExecutionImplementation();
		break;
	case ETaskStatus::ESuspended:
		HandleExecution();
		break;
	case ETaskStatus::ETerminated:
		OnTermination();
		break;
	default:
		break;
	}
}

void UNebulaFlowTaskInterface::Suspend(bool SuspendPlatform)
{
	bWasSuspended = SuspendPlatform;
	Status = ETaskStatus::ESuspended;
	SuspensionImpementation();
}

void UNebulaFlowTaskInterface::Terminate()
{
	Status = ETaskStatus::ETerminated;
}

void UNebulaFlowTaskInterface::HandleExecution()
{
	Status = ETaskStatus::EExecute;
	ExecutionImplementation();
	bWasSuspended = false;
}
