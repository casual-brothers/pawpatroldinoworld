#include "UserManagement/NebulaFlowDefineSecondaryPlayerTask.h"
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
#include "Engine/World.h"

void UNebulaFlowDefineSecondaryPlayerTask::InitializeTask(int32 iUserIndex)
{
	UserIndex = iUserIndex;
}

bool UNebulaFlowDefineSecondaryPlayerTask::IsValidImplementation()
{
	auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance)
	{
		if (gameInstance->GetLocalPlayers().Num() >= 2)
		{
			UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
			if (World)
			{
					ULocalPlayer* LocalPlayer = GEngine->GetLocalPlayerFromControllerId(World, UserIndex);
					if (LocalPlayer)
					{
						return true;
					}
			}
			return false;
		}
		auto currentGameContext = gameInstance->GetCurrentGameContext();
		return !(currentGameContext == EGameContext::EIntro || currentGameContext == EGameContext::EWelcomeScreen);
	}
	return false;
}

void UNebulaFlowDefineSecondaryPlayerTask::ExecutionImplementation()
{
	bool bSkipForward = true;
	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	check(IdentityInterface.IsValid());
	
#if PLATFORM_WINGDK
	{
		TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(0);
		FOnlineError Error(false);
		HandleLoginUIClosed(UserId, UserIndex, Error);
		bSkipForward = false;
	}
#elif (!PLATFORM_WINDOWS)
	{

		TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
		const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

		const auto LoginStatus = IdentityInterface->GetLoginStatus(UserIndex);
		if (LoginStatus == ELoginStatus::NotLoggedIn || !bIsLicensed)
		{
			// Show the account picker.
			const auto ExternalUI = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem()->GetExternalUIInterface();
			ExternalUI->SetAllowGuest(true);
			if (ExternalUI.IsValid())
			{
				ExternalUI->ShowLoginUI(UserIndex, false, true, FOnLoginUIClosedDelegate::CreateUObject(this, &ThisClass::HandleLoginUIClosed));
				bSkipForward = false;
			}
			ExternalUI->SetAllowGuest(false);
		}
	}

#endif

	if (bSkipForward)
	{
		TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(UserIndex);
		FOnlineError Error(false);
		HandleLoginUIClosed(UserId, UserIndex, Error);
	}
}

void UNebulaFlowDefineSecondaryPlayerTask::HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& Error)
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

void UNebulaFlowDefineSecondaryPlayerTask::OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		UNebulaFlowUserSubSystem* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
		if (UserSubSystem && UserSubSystem->GetPlayerOwner())
		{
			if ((PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures || PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::UserNotFound) && PendingControllerIndex.IsSet() && PendingControllerIndex.GetValue() != -1 && PendingControllerIndex.GetValue() != UserSubSystem->GetPlayerOwner()->GetControllerId())
			{
				UserSubSystem->OnUserLoggedInDelegate.Broadcast(PendingControllerIndex.GetValue());
				UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
				if (World)
				{
					UNebulaFlowLocalPlayer* LocalPlayer = Cast<UNebulaFlowLocalPlayer>(GEngine->GetLocalPlayerFromControllerId(World, PendingControllerIndex.GetValue()));
					if (LocalPlayer)
					{
						//LocalPlayer->SetCachedUniqueNetId(LocalPlayer->GetUniqueNetIdFromCachedControllerId().GetUniqueNetId());
						LocalPlayer->SetCachedUniqueNetId(UserId);

						UserSubSystem->SetSecondaryPlayerOwner(LocalPlayer);
					}
				}
				UserSubSystem->LocalPlayerOnlineStatus[UserIndex] = ELoginStatus::LoggedIn;
				PendingControllerIndex.Reset();
			}
		}
	}
	
	Terminate();
}
