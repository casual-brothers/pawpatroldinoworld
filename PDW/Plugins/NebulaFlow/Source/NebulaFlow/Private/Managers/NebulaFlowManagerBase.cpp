#include "Managers/NebulaFlowManagerBase.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowCoreDelegates.h"


void UNebulaFlowManagerBase::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	if (InstanceOwner)
	{
		GInstanceRef = InstanceOwner;
		GInstanceRef->RegisterNebulaFlowManager(this);
		GInstanceRef->OnGameContextChanged.AddDynamic(this,&UNebulaFlowManagerBase::OnGameContextChanged);
	}
	LevelLoadStartHandle = FNebulaFlowCoreDelegates::OnLevelLoadStart.AddUObject(this, &UNebulaFlowManagerBase::OnLoadLevelStarted);
	LevelLoadEndedHandle = FNebulaFlowCoreDelegates::OnLevelLoaded.AddUObject(this, &UNebulaFlowManagerBase::OnLoadLevelEnded);
}

void UNebulaFlowManagerBase::UninitManager()
{
	
	FNebulaFlowCoreDelegates::OnLevelLoadStart.Remove(LevelLoadStartHandle);
	FNebulaFlowCoreDelegates::OnLevelLoaded.Remove(LevelLoadEndedHandle);
	if (GInstanceRef)
	{
		GInstanceRef->OnGameContextChanged.RemoveDynamic(this, &UNebulaFlowManagerBase::OnGameContextChanged);
	}
}

void UNebulaFlowManagerBase::TickManager(float DeltaTime)
{
	if(!bShouldTick)
	{
		return;
	}
}

UWorld* UNebulaFlowManagerBase::GetWorld() const
{
	if (GInstanceRef)
	{
		return GInstanceRef->GetWorld();
	}
	return nullptr;
}

void UNebulaFlowManagerBase::OnLoadLevelStarted(FName LevelName)
{
	//override in derived classes
}

void UNebulaFlowManagerBase::OnLoadLevelEnded(FName LevelName)
{
	//override in derived classes
}

void UNebulaFlowManagerBase::OnGameContextChanged(EGameContext NewContext)
{
	//Override in Derived Classes
}

