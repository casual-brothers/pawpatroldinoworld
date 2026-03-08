#include "TaskManagement/NebulaFlowTaskSubsystem.h"
#include "Containers/Array.h"

void UNebulaFlowTaskSubsystem::AddTask(UNebulaFlowTaskInterface* newTask)
{
	bool bPositionFound = false;
	for (int32 i = 0; !bPositionFound && i < TaskList.Num(); i++)
	{
		if (newTask->GetPriority() > TaskList[i]->GetPriority())
		{
			if (i == 0)
			{
				auto previousTask = TaskList[i];
				if (previousTask->GetStatus() == ETaskStatus::EExecute)
				{
					previousTask->Suspend();
				}
			}
			TaskList.Insert(newTask, i);
			bPositionFound = true;
		}
	}
	if (!bPositionFound)
	{
		TaskList.Add(newTask);
	}
}

void UNebulaFlowTaskSubsystem::SuspendTasks()
{
	for (auto* Task : TaskList)
	{
		Task->Suspend(true);
	}
}

void UNebulaFlowTaskSubsystem::Tick(float DeltaTime)
{
	bool bExecuted = false;
	TArray<int32> TasksToRemove;
	for  (int32 i = 0; !bExecuted && i < TaskList.Num(); ++i)
	{
		auto currentTask = TaskList[i];
		if (currentTask->IsValid() && currentTask->GetStatus() != ETaskStatus::ETerminated)
		{
			currentTask->Execute();
			bExecuted = true;
		}
		if (currentTask->GetStatus() == ETaskStatus::ETerminated)
		{
			currentTask->Execute();
			bExecuted = true;
			TasksToRemove.Add(i);
		}
	}
	for (int32 i = TasksToRemove.Num() - 1; i >= 0; --i)
	{
		TaskList.RemoveAt(TasksToRemove[i]);
	}
}

ETickableTickType UNebulaFlowTaskSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

TStatId UNebulaFlowTaskSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTickableThing, STATGROUP_Tickables);
}
