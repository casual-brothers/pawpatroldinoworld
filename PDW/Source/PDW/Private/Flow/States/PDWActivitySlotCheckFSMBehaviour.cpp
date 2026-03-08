// Fill out your copyright notice in the Description page of Project Settings.


#include "Flow/States/PDWActivitySlotCheckFSMBehaviour.h"
#include "Managers/PDWActivitiesManager.h"
#include "FunctionLibraries/PDWActivitiesFunctionLibrary.h"

void UPDWActivitySlotCheckFSMBehaviour::OnEnter_Implementation()
{
	if (TriggeringStatus == EBehaviorTrigger::OnEnter)
	{
		if (UPDWActivitiesManager::IsActivitiesAvailableOnCurrentPlatform())
		{
			const bool bNewGame = UPDWActivitiesFunctionLibrary::GetIsNewGame(this);
			if (bNewGame)
			{
				UPDWActivitiesFunctionLibrary::ForceResetAllActivities(this, bNewGame);
			}
			else
			{
				UPDWActivitiesFunctionLibrary::StartMasterActivity(this);
				UPDWActivitiesFunctionLibrary::ForceResetAllActivities(this, bNewGame);
			}
		}
	}
}

void UPDWActivitySlotCheckFSMBehaviour::OnExit_Implementation()
{
	if (TriggeringStatus == EBehaviorTrigger::OnExit)
	{
		OnEnter_Implementation();
	}
}
