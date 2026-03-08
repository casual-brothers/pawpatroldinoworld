// Fill out your copyright notice in the Description page of Project Settings.


#include "UserManagement/NebulaFlowRemapJoyconsTask.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Managers/NebulaFlowFSMManager.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "UI/NebulaFlowDialog.h"

void UNebulaFlowRemapJoyconsTask::InitializeTask(int CurrentInGamePlayers)
{
	InGamePlayers = CurrentInGamePlayers;
}

void UNebulaFlowRemapJoyconsTask::ExecutionImplementation()
{
#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnCurrentConnectedJoyconsUpdate.AddUObject(this, &UNebulaFlowRemapJoyconsTask::OnConnectedJoyconsUpdate);
#endif
	// We ignore the pause status here because it will be managed by another task that 
// 	bWasPaused = UGameplayStatics::IsGamePaused(this);
// 	UGameplayStatics::SetGamePaused(this, true);

}

bool UNebulaFlowRemapJoyconsTask::IsValidImplementation()
{
	return true;
}

void UNebulaFlowRemapJoyconsTask::OnTermination()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
}

void UNebulaFlowRemapJoyconsTask::ContinueExecutionImplementation()
{
	FrameToWaitBeforeDialog--;

	if (FrameToWaitBeforeDialog == 0 || (RemapDialogOnScreen && !RemapDialogOnScreen->IsInViewport()))
	{
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);

		RemapDialogOnScreen = UNebulaFlowUIFunctionLibrary::ShowDialog(this, MissingControllersID,
			[=, this](FString inResponse)
			{
				OnSwitchDialogResponse(inResponse);
			}
		, UGameplayStatics::GetPlayerController(GetWorld(), 0)); //we get the first player controller always as message owner
	}

	if (FrameToWaitBeforeDialog <= 0)
	{
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	}

	if (TerminateTask)
	{
#if PLATFORM_SWITCH
		FCoreUObjectDelegates::OnCurrentConnectedJoyconsUpdate.RemoveAll(this);
#endif

		if (RemapDialogOnScreen)
		{
			UNebulaFlowUIFunctionLibrary::HideDialog(this, RemapDialogOnScreen);
			RemapDialogOnScreen = nullptr;

			if (ReturnToMainMenuDialogOnScreen)
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(this, ReturnToMainMenuDialogOnScreen);
				ReturnToMainMenuDialogOnScreen = nullptr;
			}

			UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
		}

		Super::Terminate();
		
		return;
	}

// 	if (JoyconsConnected > 0 && RemapDialogOnScreen == nullptr) // we check if controllers > 0 because the 0 case is already managed by switch by default
// 	{
// 		UGameplayStatics::SetGamePaused(this, true);
// 
// 		RemapDialogOnScreen = UNebulaFlowUIFunctionLibrary::ShowDialog(this, AreYouSureID,
// 			[=](FString inResponse)
// 		{
// 			OnSwitchDialogResponse(inResponse);
// 		}
// 		, UGameplayStatics::GetPlayerController(GetWorld(), 0)); //we get the first player controller always as message owner
// 	}
}

void UNebulaFlowRemapJoyconsTask::OnConnectedJoyconsUpdate(const int _JoyconsConnected)
{
	JoyconsConnected = _JoyconsConnected;

	if (InGamePlayers <= _JoyconsConnected)
	{
		TerminateTask = true;
	}

}

void UNebulaFlowRemapJoyconsTask::OnSwitchDialogResponse(FString Response)
{
	if (Response == "YES")
	{
		TerminateTask = true;
		UNebulaFlowConsoleFunctionLibrary::ShowRemapJoycons();
	}
	else
	{
		ReturnToMainMenuDialogOnScreen = UNebulaFlowUIFunctionLibrary::ShowDialog(this, AreYouSureID,
			[=, this](FString inResponse)
		{
			CloseConfirmationResponse(inResponse);
		}
		, UGameplayStatics::GetPlayerController(GetWorld(), 0)); //we get the first player controller always as message owner
	}
}

void UNebulaFlowRemapJoyconsTask::CloseConfirmationResponse(FString Response)
{
	if (Response == "YES")
	{
		UNebulaFlowFSMManager* FSMMAnager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(GetWorld());
		if (FSMMAnager)
		{
			FSMMAnager->SetCurrentFSMLabel("ReturnToMainMenu");
			UNebulaFlowBaseFSM* CurrentFSM = FSMMAnager->GetCurrentFSM();
			if (CurrentFSM)
			{
				UNebulaFlowCoreFunctionLibrary::SetGamePaused(this, false); // going to main menu, remove pause
				TerminateTask = true;
				CurrentFSM->GoToEntryPoint();
			}
		}
	}
	else
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, ReturnToMainMenuDialogOnScreen);
		ReturnToMainMenuDialogOnScreen = nullptr;
	}
}
