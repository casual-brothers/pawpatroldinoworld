#include "UserManagement/NebulaFlowDeviceConnectedTask.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Engine/LocalPlayer.h"
//#if (PLATFORM_XBOXONE || PLATFORM_XSX || PLATFORM_WINGDK)
//#include "GDKUserManager.h"
//#endif


void UNebulaFlowDeviceConnectedTask::InitializeTask(void* iDevice)
{
	Device = iDevice;
}

bool UNebulaFlowDeviceConnectedTask::IsValidImplementation()
{
//#TODO complience Giovannetti/Conigliaro
//#if (PLATFORM_XBOXONE || PLATFORM_XSX)
//	if (!bShowPopup)
//	{
//		return false;
//	}
//	auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
//	check(GameInstance);
//	auto CurrentGameContext = GameInstance->GetCurrentGameContext();
//	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
//	check(IdentityInterface.IsValid());
//	if (!IdentityInterface->IsDeviceConnected(Device))
//	{
//		return false;
//	}
//	int32 ControllerId = IdentityInterface->GetControllerIdFromDevice(Device);
//	ULocalPlayer* LocalPlayer = GameInstance->FindLocalPlayerFromControllerId(ControllerId);
//	int32 NumControllerFirstPlayer = 1;
//
//	NumControllerFirstPlayer = FXboxCommonPlatformInputDeviceMapper::Get().GetNumGameControllersForUser(GameInstance->GetFirstGamePlayer()->GetPlatformUserId());
//
//	return (!LocalPlayer || !LocalPlayer->IsPrimaryPlayer()) && AvailableContexts.Contains(CurrentGameContext) && NumControllerFirstPlayer > 0 && GameInstance->GetNumLocalPlayers() == 1;
//#else
//	return false;
//#endif
	return false;
}

void UNebulaFlowDeviceConnectedTask::ExecutionImplementation()
{
	if (UGameplayStatics::IsGamePaused(this))
	{
		DelayedExecution();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandler, this, &ThisClass::DelayedExecution, 0.1f, false);
	}
}

void UNebulaFlowDeviceConnectedTask::OnTermination()
{
	if (Popup)
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandler);
		GetWorld()->GetTimerManager().ClearTimer(PopupTimerHandler);
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Popup);
	}
}

void UNebulaFlowDeviceConnectedTask::SuspensionImpementation()
{
	if (Popup)
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandler);
		GetWorld()->GetTimerManager().ClearTimer(PopupTimerHandler);
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Popup);
	}
}

void UNebulaFlowDeviceConnectedTask::OnTimerExpired()
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, Popup);
	Terminate();
}

void UNebulaFlowDeviceConnectedTask::DelayedExecution()
{
	if (IsValid())
	{
		UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);

		Popup = UNebulaFlowUIFunctionLibrary::ShowDialog(this, PopupId, [=, this](FString inResponse) {}, nullptr);

		GetWorld()->GetTimerManager().SetTimer(PopupTimerHandler, this, &ThisClass::OnTimerExpired, PopupTime, false);
	}
	else 
	{
		Terminate();
	}
}

