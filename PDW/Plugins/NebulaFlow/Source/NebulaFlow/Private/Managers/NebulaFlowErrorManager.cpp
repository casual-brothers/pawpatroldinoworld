#include "Managers/NebulaFlowErrorManager.h"
#include "ErrorsManagement/NebulaFlowBaseErrorInstance.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Core/NebulaFlowGameInstance.h"



UNebulaFlowErrorManager::UNebulaFlowErrorManager(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}


UNebulaFlowBaseErrorInstance* UNebulaFlowErrorManager::CreateErrorInstanceDebug(TSubclassOf<UNebulaFlowBaseErrorInstance> ErrorInstanceClass, FName DialogID /*= NAME_None*/, UNebulaFlowLocalPlayer* inLocalPlayer /*= nullptr*/)
{
	bCreateDebugInstance = true;
	UNebulaFlowBaseErrorInstance* debugInstance = CreateErrorInstance(ErrorInstanceClass,DialogID);
	bCreateDebugInstance = false;
	return debugInstance;
}

void UNebulaFlowErrorManager::AddErrorInstance(UNebulaFlowBaseErrorInstance* AddedInstance)
{
	if (CurrentErrorInstances.Num() == 0)
	{
		CurrentErrorInstances.Add(AddedInstance);
		ManageCurrentError();
	}	
	else
	{
		int32 FoundIndex = -1;
		UNebulaFlowBaseErrorInstance* currentTopError = CurrentErrorInstances[0];
		for (int32 Index = 0; Index < CurrentErrorInstances.Num(); Index++)
		{
			if (CurrentErrorInstances[Index] != nullptr)
			{
				if (CurrentErrorInstances[Index]->ErrorInstanceType < AddedInstance->ErrorInstanceType)
				{
					FoundIndex = Index;
					break;
				}
			}
		}
		if (FoundIndex < 0)
		{
			CurrentErrorInstances.Add(AddedInstance);
		}
		else
		{
			CurrentErrorInstances.EmplaceAt(FoundIndex,AddedInstance);
		}
		if (currentTopError != CurrentErrorInstances[0])
		{
			UNebulaFlowUIFunctionLibrary::HideSystemDialog(this);
			ManageCurrentError();
		}
	}
}

void UNebulaFlowErrorManager::ManageCurrentError()
{
	if (CurrentErrorInstances.Num()>0)
	{
		if(CurrentErrorInstances[0]->IsErrorConditionValid())
		{ 
			CurrentErrorInstances[0]->StartManagingError();
		}
		else
		{
			ResolveError(CurrentErrorInstances[0]);
		}
	}
}

void UNebulaFlowErrorManager::TickManager(float DeltaTime)
{
	for (UNebulaFlowBaseErrorInstance* current : CurrentErrorInstances)
	{
		if (current && current->bShouldTick)
		{
			current->TickErrorInstance(DeltaTime);			
		}
	}
}

UNebulaFlowBaseErrorInstance* UNebulaFlowErrorManager::CreateErrorInstance(TSubclassOf<UNebulaFlowBaseErrorInstance> ErrorInstanceClass, FName DialogID, UNebulaFlowLocalPlayer* inLocalPlayer)
{
	UNebulaFlowBaseErrorInstance* NewErrorInstance = NewObject<UNebulaFlowBaseErrorInstance> (this,ErrorInstanceClass);
	UNebulaFlowLocalPlayer* PlayerOwner = inLocalPlayer!=nullptr ? inLocalPlayer : UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	if(NewErrorInstance)
	{ 
		if (bCreateDebugInstance)
		{
			NewErrorInstance->ErrorInstanceType = EErrorInstanceType::EMaxAlert;
		}
		NewErrorInstance->InitErrorInstance(this,PlayerOwner, DialogID);
		AddErrorInstance(NewErrorInstance);
		return NewErrorInstance;
	}
	return nullptr;
}

void UNebulaFlowErrorManager::ResolveError(UNebulaFlowBaseErrorInstance* inInstance)
{
	bool ShouldManageNextError = CurrentErrorInstances.Num() > 0 && CurrentErrorInstances[0] == inInstance;
	if (inInstance && CurrentErrorInstances.Contains(inInstance))
	{
		CurrentErrorInstances.Remove(inInstance);
		inInstance->UninitErrorInstance();
		inInstance->ConditionalBeginDestroy();
	}
	if (ShouldManageNextError && CurrentErrorInstances.Num() > 0)
	{
		ManageCurrentError();
	}
}
