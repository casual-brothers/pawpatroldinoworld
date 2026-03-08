// Fill out your copyright notice in the Description page of Project Settings.


#include "UserManagement/NebulaFlowAppletShownTask.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectGlobals.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"

void UNebulaFlowAppletShownTask::InitializeTask()
{
#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnRemapJoyconsUIClosed.AddUObject(this, &UNebulaFlowAppletShownTask::OnUIClosed);
#endif
}

bool UNebulaFlowAppletShownTask::IsValidImplementation()
{
	return true;
}

void UNebulaFlowAppletShownTask::OnTermination()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
}

void UNebulaFlowAppletShownTask::ExecutionImplementation()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
}

void UNebulaFlowAppletShownTask::ContinueExecutionImplementation()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);

	if (bTerminateTask)
	{

#if PLATFORM_SWITCH
		FCoreUObjectDelegates::OnRemapJoyconsUIClosed.RemoveAll(this);
#endif
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
		Super::Terminate();
	}
}

void UNebulaFlowAppletShownTask::OnUIClosed(bool success)
{
	bTerminateTask = true;
}
