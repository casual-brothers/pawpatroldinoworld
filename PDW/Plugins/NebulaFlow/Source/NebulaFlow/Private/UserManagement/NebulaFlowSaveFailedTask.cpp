#include "UserManagement/NebulaFlowSaveFailedTask.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Core/NebulaFlowGameInstance.h"

void UNebulaFlowSaveFailedTask::InitializeTask()
{

}

void UNebulaFlowSaveFailedTask::ExecutionImplementation()
{
	HandleState();
}

bool UNebulaFlowSaveFailedTask::IsValidImplementation()
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GInstance);
	return (GInstance->GetCurrentGameContext() != EGameContext::EIntro && GInstance->GetCurrentGameContext() != EGameContext::EWelcomeScreen);
}

void UNebulaFlowSaveFailedTask::SuspensionImpementation()
{
	if (Dialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		Dialog = nullptr;
	}
	bHandled = false;
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
}

void UNebulaFlowSaveFailedTask::OnTermination()
{
	if (Dialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		Dialog = nullptr;
	}
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
}

void UNebulaFlowSaveFailedTask::OnDialogResponseLoop(const FString& inResponse)
{
	if (inResponse == TEXT("Yes"))
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		Dialog = nullptr;
		UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
		if (localPlayerOwner)
		{
			bool bResult;
			localPlayerOwner->SavePersistentUser(bResult);

			if (!bResult)
			{
				//Show dialog
				//On dialog response save and check if the result is positive, if true go on otherwise show dialog again
				ShowDialogLoop();
			}
			else
			{
				UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
				Dialog = nullptr;
				FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(bResult);
				Terminate();
			}
		}
	}
	else
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		Dialog = nullptr;
		FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(false);
		Terminate();
	}
}

void UNebulaFlowSaveFailedTask::OnDialogResponseContinueWithouSaving(const FString& inResponse)
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
	Dialog = nullptr;
	FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(false);
	Terminate();
}

void UNebulaFlowSaveFailedTask::ContinueExecutionImplementation()
{
	if (!bHandled)
	{
		HandleState();
	}
}

void UNebulaFlowSaveFailedTask::ShowDialogLoop()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(playerController, DialogIdLoop,
		[=, this](FString inResponse)
	{
		OnDialogResponseLoop(inResponse);
	}
	, playerController);
}

void UNebulaFlowSaveFailedTask::ShowDialogContinueWithouSaving()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

	FName AdaptedDialogIdContinueWitoutSaving = DialogIdContinueWithouSaving;

#if defined(PLATFORM_PS5) && PLATFORM_PS5
	AdaptedDialogIdContinueWitoutSaving = DialogIdContinueWithouSavingPS5;

#elif defined(PLATFORM_PS4) && PLATFORM_PS4
	AdaptedDialogIdContinueWitoutSaving = DialogIdContinueWithouSavingPS4;

#endif

	Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(playerController, AdaptedDialogIdContinueWitoutSaving,
		[=, this](FString inResponse)
	{
		OnDialogResponseContinueWithouSaving(inResponse);
	}
	, playerController);
}

void UNebulaFlowSaveFailedTask::HandleState()
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance)
	{
		switch (GInstance->SaveCondition)
		{
		case 0:
		{
			Terminate();
			break;
		}
		case 1:
		{
			ShowDialogContinueWithouSaving();
			break;
		}
		case 2:
		{
			ShowDialogLoop();
			break;
		}
		}
	}
	bHandled = true;
}

