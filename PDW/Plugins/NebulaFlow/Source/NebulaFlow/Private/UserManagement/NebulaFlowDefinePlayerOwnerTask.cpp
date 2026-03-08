#include "UserManagement/NebulaFlowDefinePlayerOwnerTask.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "Engine/GameInstance.h"
#include "UObject/Object.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "Engine/Player.h"
#include "Core/NebulaFlowPlayerController.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/NebulaFlowDialog.h"
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
#include "GDKRuntimeModule.h"
#endif	

void UNebulaFlowDefinePlayerOwnerTask::InitializeTask(int32 userIndex)
{
	UserIndex = userIndex;
}

void UNebulaFlowDefinePlayerOwnerTask::ExecutionImplementation()
{
	bool bSkipForward = true;
	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	check(IdentityInterface.IsValid());
#if !PLATFORM_WINDOWS || PLATFORM_WINGDK
	{
		TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
		const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

		const auto LoginStatus = IdentityInterface->GetLoginStatus(UserIndex);
		bool bIsUserGuest = false;
		#if (PLATFORM_XBOXONE || PLATFORM_XSX)
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
			if (GameInstance && GameInstance->GetLocalPlayers().Num() > UserIndex)
			{
				ULocalPlayer* localPlayer = GameInstance->GetLocalPlayers()[UserIndex];
				if(localPlayer)
				{
					bIsUserGuest = IGDKRuntimeModule::Get().IsGuestUser(localPlayer->GetPlatformUserId());
				}
			}
		#endif	
		if (LoginStatus == ELoginStatus::NotLoggedIn || !bIsLicensed || bIsUserGuest)
		{
			// Show the account picker.
			const auto ExternalUI = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem()->GetExternalUIInterface();
			if (ExternalUI.IsValid())
			{
				ExternalUI->ShowLoginUI(UserIndex, false, true, FOnLoginUIClosedDelegate::CreateUObject(this, &ThisClass::HandleLoginUIClosed));
				bSkipForward = false;
			}
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

bool UNebulaFlowDefinePlayerOwnerTask::IsValidImplementation()
{
	auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance)
	{
		auto currentGameContext = gameInstance->GetCurrentGameContext();
		return (currentGameContext == EGameContext::EIntro || currentGameContext == EGameContext::EWelcomeScreen);
	}
	return false;
}

void UNebulaFlowDefinePlayerOwnerTask::HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& Error)
{
	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bool bIsLicensed = GenericApplication->ApplicationLicenseValid();
	// If they don't currently have a license, let them know, but don't let them proceed
	if (!bIsLicensed)
	{
		if (!Dialog)
		{
			Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, UnlicensedDialogId,
				[=, this](FString inResponse)
			{
				if (Dialog)
				{
					UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
					Dialog = nullptr;
				}
				Terminate();
			});
		}

		return;
	}

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
	/*else
	{
		Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, ProgressWillNotBeSavedDialogId,
			[=](FString inResponse)
		{
			if (Dialog)
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
				Dialog = nullptr;
			}
		});
	}*/
	Terminate();
}

void UNebulaFlowDefinePlayerOwnerTask::OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures && PendingControllerIndex.IsSet() && PendingControllerIndex.GetValue() != -1)
	{
		auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);	
		ANebulaFlowPlayerController* playerController = Cast< ANebulaFlowPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		check(playerController);
		UNebulaFlowLocalPlayer* NewPlayerOwner = Cast<UNebulaFlowLocalPlayer>(playerController->GetLocalPlayer());
		if (playerController)
		{
			playerController->QueryAchievements();
		}
		UNebulaFlowUserSubSystem* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
		if (NewPlayerOwner && UserSubSystem)
		{
			NewPlayerOwner->SetControllerId(PendingControllerIndex.GetValue());
			UserSubSystem->SetPlayerOwner(NewPlayerOwner);
			//NewPlayerOwner->SetCachedUniqueNetId(NewPlayerOwner->GetUniqueNetIdFromCachedControllerId().GetUniqueNetId());

			NewPlayerOwner->SetCachedUniqueNetId(UserId);

			//GetWorld()->GetAuthGameMode()->GameSession->RegisterPlayer(NewPlayerOwner->GetPlayerController(GetWorld()), NewPlayerOwner->GetPreferredUniqueNetId().GetUniqueNetId(), false);
			GetWorld()->GetAuthGameMode()->GameSession->RegisterPlayer(NewPlayerOwner->GetPlayerController(GetWorld()), NewPlayerOwner->GetPreferredUniqueNetId(), false);

			NewPlayerOwner->OnReconnectionConfirmed();
			UserSubSystem->LocalPlayerOnlineStatus[UserIndex] = ELoginStatus::LoggedIn;
			UserSubSystem->OnUserLoggedInDelegate.Broadcast(PendingControllerIndex.GetValue());
		}
		PendingControllerIndex.Reset();
		Terminate();
	}
}