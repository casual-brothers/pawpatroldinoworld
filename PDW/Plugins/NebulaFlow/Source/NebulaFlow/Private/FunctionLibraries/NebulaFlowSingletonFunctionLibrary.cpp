#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"

#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NebulaFlow.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"


UNebulaFlowFSMManager* UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		if (GInstance->FSMManagerInstance == nullptr)
		{
			ensure(GInstance->FlowFSMManagerClass);
			if (!GInstance->FlowFSMManagerClass)
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("NO Class Definition for FSM Manager!"));
				return nullptr;
			}
			GInstance->FSMManagerInstance = NewObject<UNebulaFlowFSMManager>(GInstance,GInstance->FlowFSMManagerClass,FName("FSMMAnager"));
			if (GInstance->FSMManagerInstance)
			{
				GInstance->FSMManagerInstance->InitManager(GInstance);
			}
		}
		return GInstance->FSMManagerInstance;		
	}
	return nullptr;
}

UNebulaFlowUIManager* UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		if (GInstance->UIManagerInstance == nullptr)
		{
			ensure(GInstance->FlowUIManagerClass);
			if (!GInstance->FlowUIManagerClass)
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("NO Class Definition for UI Manager!"));
				return nullptr;
			}
			GInstance->UIManagerInstance = NewObject<UNebulaFlowUIManager>(GInstance, GInstance->FlowUIManagerClass, FName("UIManager"));
			if (GInstance->UIManagerInstance)
			{
				GInstance->UIManagerInstance->InitManager(GInstance);
			}
		}
		return GInstance->UIManagerInstance;
	}
	return nullptr;
}

UNebulaFlowErrorManager* UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowErrorManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		if (GInstance->ErrorManagerInstance == nullptr)
		{
			ensure(GInstance->FlowErrorManagerClass);
			if (!GInstance->FlowErrorManagerClass)
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("NO Class Definition for Error Manager!"));
				return nullptr;
			}
			GInstance->ErrorManagerInstance = NewObject<UNebulaFlowErrorManager>(GInstance, GInstance->FlowErrorManagerClass, FName("ErrorManager"));
			if (GInstance->ErrorManagerInstance)
			{
				GInstance->ErrorManagerInstance->InitManager(GInstance);
			}
		}
		return GInstance->ErrorManagerInstance;
	}
	return nullptr;
}

UNebulaFlowCommandManager* UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowCommandManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		if (GInstance->CommandManagerInstance == nullptr)
		{
			GInstance->CommandManagerInstance = NewObject<UNebulaFlowCommandManager>(GInstance, UNebulaFlowCommandManager::StaticClass(), FName("CommandManager"));
		}
		return GInstance->CommandManagerInstance;
	}
	return nullptr;
}

UNebulaFlowAchievementManager* UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAchievementManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		if (GInstance->AchievementManagerInstance == nullptr)
		{
			ensure(GInstance->FlowAchievementManagerClass);
			if (!GInstance->FlowAchievementManagerClass)
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("NO Class Definition for Achievement Manager!"));
				return nullptr;
			}
			GInstance->AchievementManagerInstance = NewObject<UNebulaFlowAchievementManager>(GInstance, GInstance->FlowAchievementManagerClass, FName("AchievementManager"));
			if (GInstance->AchievementManagerInstance)
			{
				GInstance->AchievementManagerInstance->InitManager(GInstance);
			}
		}
		return GInstance->AchievementManagerInstance;
	}
	return nullptr;
}

UNebulaFlowAudioManager* UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		if (GInstance->AudioManagerInstance == nullptr)
		{
			ensure(GInstance->FlowAudioManagerClass);
			if (!GInstance->FlowAudioManagerClass)
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("NO Class Definition for Audio Manager!"));
				return nullptr;
			}
			GInstance->AudioManagerInstance = NewObject<UNebulaFlowAudioManager>(GInstance, GInstance->FlowAudioManagerClass, FName("AudioManager"));
			if (GInstance->AudioManagerInstance)
			{
				GInstance->AudioManagerInstance->InitManager(GInstance);
			}
		}
		return GInstance->AudioManagerInstance;
	}
	return nullptr;
}

UNebulaFlowUserSubSystem* UNebulaFlowSingletonFunctionLibrary::GetUserSubsystem(const UObject* WorldContextObject)
{
	auto* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	}
	return nullptr;
}

// UNebulaFlowUserSubSystem* UNebulaFlowSingletonFunctionLibrary::GetUserSubsystemNOCONST(UObject* WorldContextObject)
// {
// 	return nullptr;
// }
