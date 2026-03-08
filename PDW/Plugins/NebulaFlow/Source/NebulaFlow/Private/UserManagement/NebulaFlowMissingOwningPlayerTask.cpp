#include "UserManagement/NebulaFlowMissingOwningPlayerTask.h"
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
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
#include "XboxCommonPlatformApplicationMisc.h"
#include "XboxCommonApplication.h"
#include "XboxCommonPlatformInputDeviceMapper.h"
#endif

void UNebulaFlowMissingOwningPlayerTask::InitializeTask(int32 localUserNum, const FControllerPairingChangedUserInfo& previousUser, const FControllerPairingChangedUserInfo& newUser)
{
	Super::InitializeTask(localUserNum, previousUser, newUser);
}

void UNebulaFlowMissingOwningPlayerTask::ExecutionImplementation()
{
	ShowDialog();
}

bool UNebulaFlowMissingOwningPlayerTask::IsValidImplementation()
{
	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	check(IdentityInterface.IsValid());
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GInstance);
	if (GInstance->GetCurrentGameContext() == EGameContext::EIntro || GInstance->GetCurrentGameContext() == EGameContext::EWelcomeScreen)
	{
		return false;
	}
	int32 NumControllers = 1;
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	if (PreviousPlatformUserId.IsValid())
	{
		NumControllers = FXboxCommonPlatformInputDeviceMapper::Get().GetNumGameControllersForUser(PreviousPlatformUserId);
	}
#endif
	bool bIsValid = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetControllerId() == PreviousUserId && NumControllers < 1;
	if (bIsValid)
	{
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	}
	return bIsValid;
}

void UNebulaFlowMissingOwningPlayerTask::OnTermination()
{
	if (SystemDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this, SystemDialog);
		SystemDialog = nullptr;
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
	}
}

void UNebulaFlowMissingOwningPlayerTask::ShowDialog()
{

#if (PLATFORM_XBOXONE || PLATFORM_XSX)

	
		FPlatformUserId PlatformId = FPlatformMisc::GetPlatformUserForUserIndex(0);

		FShowInputDeviceSelectorComplete OnShowInputDeviceSelectorComplete = ([this](const FShowInputDeviceSelectorParams&) {		
			UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
			Terminate();
		});

		if (!FXboxCommonPlatformApplicationMisc::ShowInputDeviceSelector(PlatformId, (OnShowInputDeviceSelectorComplete)))
		{
			UE_LOG(LogNebulaFlowUI, Warning, TEXT("Failed To Open Input Device Selector UI"));
		}
		return;	
#else

	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	SystemDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(playerController, DialogId,
		[=, this](FString inResponse)
	{
		OnDialogChoice(inResponse);
	}
	, playerController);
#endif
}

void UNebulaFlowMissingOwningPlayerTask::SendToWelcomePage()
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, SystemDialog);
	//UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	//if (localPlayerOwner)
	//{
	//	bool bResult;
	//	localPlayerOwner->SavePersistentUser(bResult);
	//}
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
	Terminate();
}

void UNebulaFlowMissingOwningPlayerTask::OnDialogChoice(const FString& inResponse)
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

