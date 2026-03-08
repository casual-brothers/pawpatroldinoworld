#include "UserManagement/NebulaFlowRedefinePlayerOwnerTask.h"
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
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowFSMManager.h"

void UNebulaFlowRedefinePlayerOwnerTask::InitializeTask(int32 userIndex)
{
	UserIndex = userIndex;
}

void UNebulaFlowRedefinePlayerOwnerTask::ExecutionImplementation()
{
	bool bSkipForward = true;
#if !PLATFORM_WINDOWS || PLATFORM_WINGDK
	{
		// Show the account picker.
		const auto ExternalUI = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem()->GetExternalUIInterface();
		if (ExternalUI.IsValid())
		{
			ExternalUI->ShowLoginUI(UserIndex, false, true, FOnLoginUIClosedDelegate::CreateUObject(this, &ThisClass::HandleLoginUIClosed));
			bSkipForward = false;
		}
	}

#endif

	if (bSkipForward)
	{
		const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
		check(IdentityInterface.IsValid());
		TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(UserIndex);
		FOnlineError Error(false);
		HandleLoginUIClosed(UserId, UserIndex, Error);
	}
}

bool UNebulaFlowRedefinePlayerOwnerTask::IsValidImplementation()
{
	auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance)
	{
		auto currentGameContext = gameInstance->GetCurrentGameContext();
		return !(currentGameContext == EGameContext::EIntro || currentGameContext == EGameContext::EWelcomeScreen);
	}
	return false;
}

void UNebulaFlowRedefinePlayerOwnerTask::HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& Error)
{
	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

	// If they don't currently have a license, let them know, but don't let them proceed
	if (!bIsLicensed)
	{
		// #TODO_Licenses Add not licensed System DIALOG
		Terminate();
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
	else
	{
		auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);
		UNebulaFlowUserSubSystem* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
		if (UserSubSystem)
		{
			UserSubSystem->RedefinePlayerOwner(UserIndex);
		}
	}
	Terminate();
}

void UNebulaFlowRedefinePlayerOwnerTask::OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures && PendingControllerIndex.IsSet() && PendingControllerIndex.GetValue() != -1)
	{
		auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);	
		//if (*GameInstance->GetPrimaryPlayerUniqueId().Get() == UserId)
		if (*GameInstance->GetPrimaryPlayerUniqueIdRepl() == UserId)
		{
			UNebulaFlowUserSubSystem* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
			if (UserSubSystem)
			{
				UserSubSystem->GetPlayerOwner()->SetControllerId(PendingControllerIndex.GetValue());
				UserSubSystem->GetPlayerOwner()->OnReconnectionConfirmed();
				PendingControllerIndex.Reset();
			}
			Terminate();
		} 
		else
		{
			SendToWelcomePage();
		}
	}
}

void UNebulaFlowRedefinePlayerOwnerTask::SendToWelcomePage()
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	ensureMsgf(GInstance && GInstance->WelcomeScreenMapName != NAME_None, TEXT("WelcomeScreenMap Name not defined in GameInstance!"));
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);

	if (GInstance && GInstance->WelcomeScreenMapName != NAME_None)
	{
		UNebulaFlowFSMManager* FSMManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(this);
		if (FSMManager)
		{
			FSMManager->SetCurrentFSMLabel(TEXT("Default"));
		}
		UNebulaFlowCoreFunctionLibrary::LoadLevelByName(this, GInstance->WelcomeScreenMapName);
	}
	else
	{
		SendToWelcomePage();
	}
	Terminate();
}
