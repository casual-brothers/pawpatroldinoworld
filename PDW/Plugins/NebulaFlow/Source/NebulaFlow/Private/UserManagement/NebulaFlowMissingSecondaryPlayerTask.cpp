#include "UserManagement/NebulaFlowMissingSecondaryPlayerTask.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UserManagement/NebulaFlowUserTaskConfiguration.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "Engine/DataTable.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
#include "GDKRuntimeModule.h"
#include "XboxCommonPlatformInputDeviceMapper.h"
#endif
#include "Core/NebulaFlowCoreDelegates.h"

void UNebulaFlowMissingSecondaryPlayerTask::InitializeTask(int32 localUserNum, const FControllerPairingChangedUserInfo& previousUser, const FControllerPairingChangedUserInfo& newUser)
{
	Super::InitializeTask(localUserNum, previousUser, newUser);
}

void UNebulaFlowMissingSecondaryPlayerTask::ExecutionImplementation()
{
	FNebulaFlowCoreDelegates::OnUserLogOut.Broadcast(PreviousUserId);
	ShowDialog();
}

bool UNebulaFlowMissingSecondaryPlayerTask::IsValidImplementation()
{
	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	check(IdentityInterface.IsValid());
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GInstance);
	int32 NumControllers = 1;
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	if (PreviousPlatformUserId.IsValid())
	{
		NumControllers = FXboxCommonPlatformInputDeviceMapper::Get().GetNumGameControllersForUser(PreviousPlatformUserId);
	}
#endif
	auto currentGameContext = GInstance->GetCurrentGameContext();
	if (!(currentGameContext != EGameContext::EIntro && currentGameContext != EGameContext::EWelcomeScreen))
	{
		return false;
	}
	bool bIsValid = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetControllerId() != PreviousUserId && UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetControllerId() != NextUserId 	&& NumControllers < 1;
	return bIsValid;
}

void UNebulaFlowMissingSecondaryPlayerTask::OnTermination()
{
	if (PopupDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, PopupDialog);
		PopupDialog = nullptr;
	}
}

void UNebulaFlowMissingSecondaryPlayerTask::OnTimerExpired()
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, PopupDialog);
	Terminate();
}

void UNebulaFlowMissingSecondaryPlayerTask::ShowDialog()
{
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	if (IGDKRuntimeModule::Get().IsUsingSimplifiedUserModel())
	{
		return;
	}
#endif
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	PopupDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(playerController, DialogId,{}, nullptr);
	GetWorld()->GetTimerManager().SetTimer(PopupTimerHandler, this, &ThisClass::OnTimerExpired, PopupTime, false);
}

