#include "UserManagement/NebulaFlowHandleLoginTask.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Managers/NebulaFlowFSMManager.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "TimerManager.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Actors/GameModes/NebulaFlowBaseGameMode.h"
#include "Engine/Player.h"
#include "GameFramework/PlayerController.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformMisc.h"

const FName UNebulaFlowHandleLoginTask::FROM_PROFILE_CHANGED_LABEL = FName("Default");

void UNebulaFlowHandleLoginTask::InitializeTask(int32 gameUserIndex, ELoginStatus::Type previousLoginStatus, ELoginStatus::Type loginStatus, const FUniqueNetId& userId)
{
	GameUserIndex = gameUserIndex;
	PreviousLoginStatus = previousLoginStatus;
	LoginStatus = loginStatus;

	UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(this);
	if (gameInstance)
	{
		//Previous: LocalPlayer = gameInstance->FindLocalPlayerFromUniqueNetId(userId);
		LocalPlayer = gameInstance->GetLocalPlayerByIndex(GameUserIndex);
	}
	// On Switch, accounts can play in LAN games whether they are signed in online or not.
#if PLATFORM_SWITCH
	bDowngraded = LoginStatus == ELoginStatus::NotLoggedIn/* || (GetOnlineMode() == EOnlineMode::Online && LoginStatus == ELoginStatus::UsingLocalProfile)*/;
#else
	bDowngraded = LoginStatus == ELoginStatus::NotLoggedIn /*(LoginStatus == ELoginStatus::NotLoggedIn && GetOnlineMode() == EOnlineMode::Offline) || (LoginStatus != ELoginStatus::LoggedIn && GetOnlineMode() != EOnlineMode::Offline)*/;
#endif
}

bool UNebulaFlowHandleLoginTask::IsValidImplementation()
{
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	
		return false;
#else

	auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance)
	{
		auto currentGameContext = gameInstance->GetCurrentGameContext();
		return !(currentGameContext == EGameContext::EIntro || currentGameContext == EGameContext::EWelcomeScreen);
	}

	return false;
#endif
}

void UNebulaFlowHandleLoginTask::OnTermination()
{
	if (Dialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		Dialog = nullptr;
	}
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
	UGameplayStatics::GetGameInstance(this)->GetTimerManager().ClearTimer(TimerHandler);

}

void UNebulaFlowHandleLoginTask::ExecutionImplementation()
{
	UE_LOG(LogOnline, Log, TEXT("HandleUserLoginChanged: bDownGraded: %i"), (int)bDowngraded);

	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

	// Find the local player associated with this unique net id
	UNebulaFlowUserSubSystem* UserSubSystem = UNebulaFlowSingletonFunctionLibrary::GetUserSubsystem(this);
	check(UserSubSystem);
	if (!bWasSuspended && UserSubSystem->LocalPlayerOnlineStatus[GameUserIndex] == LoginStatus)
	{
		Terminate();
		return;
	}
	UserSubSystem->LocalPlayerOnlineStatus[GameUserIndex] = LoginStatus;

	if (LocalPlayer && LoginStatus == ELoginStatus::LoggedIn)
	{
		ANebulaFlowPlayerController* PController = Cast< ANebulaFlowPlayerController>(LocalPlayer->GetPlayerController(GetWorld()));
		if (PController)
		{
			PController->QueryAchievements();

			if (TObjectPtr<APawn>  Pawn = PController->GetPawn())
			{
				Pawn->EnableInput(PController);
			}
		}
	}

	// If this user is signed out, but was previously signed in, punt to welcome (or remove splitscreen if that makes sense)
	if (LocalPlayer != nullptr)
	{
		if (bDowngraded)
		{
			if (LocalPlayer->GetPreferredUniqueNetId().IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("HandleUserLoginChanged: Player logged out: %s"), *(*LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId()).ToString());
			}
			// Check to see if this was the master, or if this was a split-screen player on the client
			if (UserSubSystem->LogginOutInProgress.IsSet())
			{
				if (UserSubSystem->LogginOutInProgress->PreviousLoginStatus == ELoginStatus::LoggedIn
					&& UserSubSystem->LogginOutInProgress->LoginStatus == ELoginStatus::NotLoggedIn
					&& LoginStatus == ELoginStatus::LoggedIn)
				{
					auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
					if (gameInstance)
					{
						gameInstance->GetTimerManager().ClearTimer(UserSubSystem->LogginOutInProgresseTimerHandle);

					}
					UserSubSystem->LogginOutInProgress.Reset();
				}
				else
				{
					UserSubSystem->LogginOutInProgress.Reset();
					if (Dialog)
					{
						UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
					}
					if (UGameplayStatics::IsGamePaused(this))
					{
						HandleUserLoginChangedDelayed();
					}
					else
					{
						UserSubSystem->LogginOutInProgress = FFlowUserLoginChanged(GameUserIndex, PreviousLoginStatus, LoginStatus/*, *LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId()*/);
						auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
						if (gameInstance)
						{
							gameInstance->GetTimerManager().SetTimer(UserSubSystem->LogginOutInProgresseTimerHandle, this, &ThisClass::HandleUserLoginChangedDelayed, UserLoginChangedDelay, false);
						}
					}
					return;
				}
			}
			else
			{

				if (UGameplayStatics::IsGamePaused(this))
				{
					HandleUserLoginChangedDelayed();
				}
				else
				{
					UserSubSystem->LogginOutInProgress = FFlowUserLoginChanged(GameUserIndex, PreviousLoginStatus, LoginStatus/*, *LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId()*/);
					auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
					if (gameInstance)
					{
						gameInstance->GetTimerManager().SetTimer(UserSubSystem->LogginOutInProgresseTimerHandle, this, &ThisClass::HandleUserLoginChangedDelayed, UserLoginChangedDelay, false);
					}
				}
				return;
			}
		}
	}
	Terminate();
}

void UNebulaFlowHandleLoginTask::OnDialogResponse(const FString& inResponse)
{
	//#if !PLATFORM_WINDOWS || PLATFORM_WINGDK
	//	FGenericPlatformMisc::RequestExit(false);
	//#else
	UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
	if (ForcePause)
	{
		if (inResponse == SendToWelcomeAction)
		{
			SendToWelcomePage();
		}
		else if (inResponse == ProfileSwapAction)
		{
			UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
			if (GInstance)
			{
				UNebulaFlowUserSubSystem* UserSubSystem = GInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
				check(UserSubSystem);
				UserSubSystem->RedefinePlayerOwner(UserSubSystem->GetPlayerOwner()->GetControllerId());
				Terminate();
			}
		}
	}

	UNebulaFlowUserSubSystem* UserSubSystem = UNebulaFlowSingletonFunctionLibrary::GetUserSubsystem(this);
	check(UserSubSystem);
	UserSubSystem->LogginOutInProgress.Reset();
	Terminate();
	//#endif
}

void UNebulaFlowHandleLoginTask::HandleUserLoginChangedDelayed()
{
	FNebulaFlowCoreDelegates::OnUserLogOut.Broadcast(GameUserIndex);
	FName DialogId;
#if PLATFORM_WINGDK
	//DialogId = WinGDKDialogId;
	FGenericPlatformMisc::RequestExit(false);
	return;
#else
	DialogId = UserLoginDialogId;
	Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, DialogId,
		[=, this](FString inResponse)
		{
			OnDialogResponse(inResponse);
		}
	, LocalPlayer->PlayerController);
	if (ForcePause)
	{
		bShouldPause = true;
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	}
	if (Dialog && !ErrorText.IsEmpty())
	{
		Dialog->SetDialogText(ErrorText);
	}
	if (!ForcePause)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ThisClass::OnTimerExpired, PopupTime, false);
	}
	//LocalPlayer->SetCachedUniqueNetId(nullptr);
	APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
	if (!PlayerController)
	{
		PlayerController = LocalPlayer->GetPlayerController(nullptr);
	}
	ensure(PlayerController);
	if (PlayerController)
	{
		PlayerController->DisableInput(nullptr);
		if (PlayerController->GetPawn())
		{
			PlayerController->GetPawn()->DisableInput(PlayerController);
		}
	}
#endif
}

void UNebulaFlowHandleLoginTask::OnTimerExpired()
{
	OnDialogResponse({});
}

void UNebulaFlowHandleLoginTask::SendToWelcomePage()
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	ensureMsgf(GInstance && GInstance->WelcomeScreenMapName != NAME_None, TEXT("WelcomeScreenMap Name not defined in GameInstance!"));
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
	if (GInstance && GInstance->WelcomeScreenMapName != NAME_None)
	{
		UNebulaFlowFSMManager* FSMManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(this);
		if (FSMManager)
		{
			FSMManager->SetCurrentFSMLabel(ReturnFsmLabel);
		}
		UNebulaFlowCoreFunctionLibrary::LoadLevelByName(this, GInstance->WelcomeScreenMapName);
	}
}