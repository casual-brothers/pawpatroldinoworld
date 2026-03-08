#include "Managers/NebulaFlowIntentManager.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"




DEFINE_LOG_CATEGORY(LogPlayerIntents);

UNebulaFlowIntentManager* UNebulaFlowIntentManager::GetFlowIntentManager(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));

	if (GInstance && GInstance->FlowIntentManagerInstance)
	{
		UNebulaFlowIntentManager* Manager = GInstance->FlowIntentManagerInstance;

		return (Manager && Manager->bHasBeenInitialized) ? GInstance->FlowIntentManagerInstance : nullptr;
	}

	UE_LOG(LogPlayerIntents, Warning, TEXT("Intent Manager has not been initialized"));

	return nullptr;
}

TArray<FName> UNebulaFlowIntentManager::GetCurrentManagedIntents() const
{
	return CurrentManagedIntents;
}

void UNebulaFlowIntentManager::OnActivityIntentReceived(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineSessionSearchResult* SessionInfo)
{
	if (bCanManageintent)
	{
		OnActivityIntent_Handler(LocalUserId, ActivityId, SessionInfo);
	}
}

void UNebulaFlowIntentManager::Init()
{
	//if (!UFlowActivitiesManager::IsActivitiesAvailableOnCurrentPlatform())
	//{
		//return;
	//}

	if (!bHasBeenInitialized)
	{
		bHasBeenInitialized = true;

		InitOfflineActivityIntents();
		InitOnlineActivityIntents();
		IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
		if (OnlineSub && OnlineSub->GetGameActivityInterface())
		{
			FOnGameActivityActivationRequestedDelegate ActivityActivationIntentIntentHandler = FOnGameActivityActivationRequestedDelegate::CreateUObject(this, &UNebulaFlowIntentManager::OnActivityIntentReceived);
			OnlineSub->GetGameActivityInterface()->AddOnGameActivityActivationRequestedDelegate_Handle(ActivityActivationIntentIntentHandler);

		}
	}
	UE_LOG(LogPlayerIntents, Warning, TEXT("Intent Manager init end"));
}

UNebulaFlowIntentManager::UNebulaFlowIntentManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UNebulaFlowIntentManager::InitOfflineActivityIntents()
{
	//Override in derived classes (Game specific logics): For each Intent the specific game can handle, in overridden function use UFlowIntentManager::DefineNewIntent() function to define them

}

void UNebulaFlowIntentManager::InitOnlineActivityIntents()
{
	//Override in Derived Classes (Game Specific Logics)
}

void UNebulaFlowIntentManager::OnActivityIntent_Handler(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineSessionSearchResult* SessionInfo)
{
	//OverrideIn Derived Classes (Game Specific logics)
}
