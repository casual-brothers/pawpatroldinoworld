#pragma once

#include "UObject/ScriptInterface.h"
#include "NebulaFlowTaskInterface.h"
#include "CoreMinimal.h"
#include "Tickable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UserManagement/NebulaFlowUserTaskConfiguration.h"
#include "NebulaFlowTaskSubsystem.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowTaskSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:

	void AddTask(UNebulaFlowTaskInterface* newTask);

	template <typename TaskType, typename...TaskArgumentTypes>
	static TaskType* CreateTask(UObject* worldContext, UGameInstance* GameInstance, FNebulaFlowUserTaskConfiguration* userTaskConfiguration, TaskArgumentTypes&&... taskArguments);

	TArray<UNebulaFlowTaskInterface *>& GetTaskList() { return TaskList; }
	void SuspendTasks();

protected:

	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;	
	virtual TStatId GetStatId() const override;
private:

	UPROPERTY()
	TArray<UNebulaFlowTaskInterface*> TaskList;
};

template <typename TaskType, typename... TaskArgumentTypes>
TaskType* UNebulaFlowTaskSubsystem::CreateTask(UObject* worldContext, UGameInstance* GameInstance, FNebulaFlowUserTaskConfiguration* userTaskConfiguration, TaskArgumentTypes&&... taskArguments)
{	
	TaskType* task = nullptr;
	if (userTaskConfiguration)
	{
		auto* taskSubsystem = GameInstance->GetSubsystem<UNebulaFlowTaskSubsystem>();
		check(taskSubsystem);
		task = NewObject<TaskType>(worldContext, userTaskConfiguration->TaskClass);
		check(task);
		task->SetPriority(userTaskConfiguration->Priority);
		task->InitializeTask(Forward<TaskArgumentTypes>(taskArguments)...);
		taskSubsystem->AddTask(task);
	}	
	return task;
}
