#include "Managers/NebulaFlowFSMManager.h"
#include "Core/NebulaFlowGameInstance.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "FSM/NebulaFlowBaseFSMState.h"



UNebulaFlowFSMManager::UNebulaFlowFSMManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowFSMManager::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	Super::InitManager(InstanceOwner);
	CurrentRunningFSMs.Empty();
	CurrentFSMLabel.Reset();
}



bool UNebulaFlowFSMManager::GetCurrentFSMLabel(FName& OutLabel, bool CleanAfterRead /*= true*/)
{	
	if (CurrentFSMLabel.IsSet())
	{
		OutLabel = CurrentFSMLabel.GetValue();
		if (CleanAfterRead)
		{
			CurrentFSMLabel.Reset();
		}
		return true;
	}

	return false;	
}

FName UNebulaFlowFSMManager::GetCurrentFsmStateId()
{
	if (CurrentRunningFSMs.Num() > 0)
	{
		return CurrentRunningFSMs[0]->CurrentFsmState->StateName;
	}
	return NAME_None;
}

