#include "UserManagement/NebulaFlowChangeUserTask.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "GenericPlatform/GenericApplication.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"

void UNebulaFlowChangeUserTask::InitializeTask(int32 iUserIndex)
{
	UserIndex = iUserIndex;
}

bool UNebulaFlowChangeUserTask::IsValidImplementation()
{
	auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance)
	{
		auto currentGameContext = gameInstance->GetCurrentGameContext();
		return !(currentGameContext == EGameContext::EIntro || currentGameContext == EGameContext::EWelcomeScreen);
	}
	return false;
}

void UNebulaFlowChangeUserTask::ExecutionImplementation()
{
	bool bSkipForward = true;
	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	check(IdentityInterface.IsValid());
#if !PLATFORM_WINDOWS || PLATFORM_WINGDK
	{
		const auto ExternalUI = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem()->GetExternalUIInterface();
		if (ExternalUI.IsValid())
		{
			ExternalUI->ShowLoginUI(UserIndex, false, true, FOnLoginUIClosedDelegate::CreateUObject(this, &ThisClass::HandleLoginUIClosed));
			bSkipForward = false;
		}
	}
#endif
}

void UNebulaFlowChangeUserTask::HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& Error)
{
	PendingControllerIndex = ControllerIndex;

	if (UniqueId.IsValid())
	{
		// Next step, check privileges
		const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
		if (IdentityInterface.IsValid())
		{
			IdentityInterface->GetUserPrivilege(*UniqueId, EUserPrivileges::CanPlay, IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &ThisClass::OnUserCanPlay));
			return;
		}
	}
	Terminate();
}

void UNebulaFlowChangeUserTask::OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	if (IsValidImplementation())	
	{
		if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures && PendingControllerIndex.IsSet() && PendingControllerIndex.GetValue() != -1)
		{
			auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
			check(GameInstance);
			UNebulaFlowUserSubSystem* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
			if (UserSubSystem)
			{
				UserSubSystem->OnUserLoggedInDelegate.Broadcast(PendingControllerIndex.GetValue());
			}
			PendingControllerIndex.Reset();
		}
	}
	Terminate();
}