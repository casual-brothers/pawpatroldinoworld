#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"

#include "HAL/ThreadHeartBeat.h"
#include "DeviceProfiles/DeviceProfileManager.h"

bool UNebulaFlowConsoleFunctionLibrary::bIsJoyConHorizontal = false;

void UNebulaFlowConsoleFunctionLibrary::SetIsJoyconHorizontal()
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (ConnectedControllerTypes[0] == FSwitchPlatformMisc::ENpadControllerType::Npad_LeftJoyCon || ConnectedControllerTypes[0] == FSwitchPlatformMisc::ENpadControllerType::Npad_RightJoyCon)
	{
		bIsJoyConHorizontal = true;
	}
	else
	{
		bIsJoyConHorizontal = false;
	}
#endif
}

bool UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(const int32 InID)
{
#if PLATFORM_SWITCH
	return bIsJoyConHorizontal;
	//FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	//if (InID < 8 && (ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_LeftJoyCon || ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_RightJoyCon))
	//{
	//	return true;
	//}
	//return false;
#else
	return false;
#endif
}

bool UNebulaFlowConsoleFunctionLibrary::IsJoyconDualGrip(const int32 InID)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (InID < 8 && ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_DualJoyCon)
	{
		return true;
	}
	return false;
#else
	return false;
#endif
}

bool UNebulaFlowConsoleFunctionLibrary::IsJoyconHandheld(const int32 InID)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (InID < 8 && ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_Handheld)
	{
		return true;
	}
	return false;
#else
	return false;
#endif
}

bool UNebulaFlowConsoleFunctionLibrary::IsJoyconLeft(const int32 InID)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (InID < 8 && ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_LeftJoyCon)
	{
		return true;
	}
	return false;
#else
	return false;
#endif
}

bool UNebulaFlowConsoleFunctionLibrary::IsJoyconRight(const int32 InID)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (InID < 8 && ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_RightJoyCon)
	{
		return true;
	}
	return false;
#else
	return false;
#endif
}

bool UNebulaFlowConsoleFunctionLibrary::IsSwitchProController(const int32 InID)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (InID < 8 && ConnectedControllerTypes[InID] == FSwitchPlatformMisc::ENpadControllerType::Npad_Pro)
	{
		return true;
	}
	return false;
#else
	return false;
#endif
}

void UNebulaFlowConsoleFunctionLibrary::ShowRemapJoycons()
{
	// We need to take care of the hang on the heartbeat.
	// Suspending the heartbeat check which otherwise would call a crash on hang.

	FThreadHeartBeat::Get().SetDurationMultiplier(1000);
#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnRemapJoyconsUIOpen.Broadcast();
#endif
	GEngine->DeferredCommands.Add(TEXT("Npad.ConnectUI"));
}

void UNebulaFlowConsoleFunctionLibrary::SetEnableSingleJoycon(const bool InIsEnable)
{
	if (InIsEnable)
	{
		GEngine->DeferredCommands.Add(TEXT("Npad.EnableSingleStick 1"));
	}
	else
	{
		GEngine->DeferredCommands.Add(TEXT("Npad.EnableSingleStick 0"));
	}
}

void UNebulaFlowConsoleFunctionLibrary::SetSwitchMinControllers(const int32 InNum)
{
	GEngine->DeferredCommands.Add(TEXT("Npad.MinControllers ") + FString::FromInt(InNum));
}

void UNebulaFlowConsoleFunctionLibrary::SetSwitchMaxControllers(const int32 InNum)
{
	GEngine->DeferredCommands.Add(TEXT("Npad.MaxControllers ") + FString::FromInt(InNum));
}

void UNebulaFlowConsoleFunctionLibrary::ResetHeartBeatDurationMultiplier()
{
	FThreadHeartBeat::Get().SetDurationMultiplier(1);
}

bool UNebulaFlowConsoleFunctionLibrary::IsJoyconConnected(const int32 InID)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::ENpadControllerType* ConnectedControllerTypes = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite();
	if (InID < 8 && ConnectedControllerTypes[InID])
	{
		return true;
	}
	return false;
#else
	return false;
#endif
}