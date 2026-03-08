#include "UserManagement/NebulaFlowControllerConnectionChangeTask.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/StreamableManager.h"
#include "UObject/UObjectGlobals.h"
#include "Core/NebulaFlowPlayerController.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "NebulaFlow.h"
#if (PLATFORM_XBOXONE || PLATFORM_XSX || PLATFORM_WINGDK)

#include "../Platforms/GDK/Source/Runtime/GDKRuntime/Public/GDKRuntimeModule.h"
//#include "GDKUserManager.h"
////#include "GDKPlatformInputDeviceMapper.h"
#endif
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
#include "XboxCommonPlatformApplicationMisc.h"
#include "XboxCommonApplication.h"
#include "XboxCommonPlatformInputDeviceMapper.h"
#endif
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"

void UNebulaFlowControllerConnectionChangeTask::InitializeTask(bool bIsConnected, FPlatformUserId userId, int32 inputDeviceId)
{
	IsConnected = bIsConnected;
	UserId = userId;
	InputDeviceId = inputDeviceId;
}

void UNebulaFlowControllerConnectionChangeTask::ExecutionImplementation()
{
	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);
	UNebulaFlowLocalPlayer* LocalPlayer = Cast<UNebulaFlowLocalPlayer>(GameInstance->FindLocalPlayerFromPlatformUserId(UserId));
	if (LocalPlayer)
	{
		const IOnlineIdentityPtr& identityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
		check(identityInterface.IsValid());
		if (IsConnected)
		{
			if (LocalPlayer->IsWaitingForReconnectionConfirm())
			{
				LocalPlayer->UpdateConnectionStatus(IsConnected, false);
			}
		}
		else
		{
			FNebulaFlowCoreDelegates::OnControllerDisconnect.Broadcast(LocalPlayer->GetControllerId());
			if (PauseOnDisconnect)
			{
				LocalPlayer->UpdateConnectionStatus(IsConnected, true);
				ShowDialogOnPause();
			}
			else
			{	
				LocalPlayer->UpdateConnectionStatus(IsConnected, true);
				ShowDialogWithoutPause();
			}			
			return;
		}
	}
	#if (PLATFORM_PS4 || PLATFORM_PS5)
	else
	{
		ShowDialogWithoutPause();
	}
	#endif
	Terminate();
}

bool UNebulaFlowControllerConnectionChangeTask::IsValidImplementation()
{
	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);

	bool bIsUsingSimplifiedUserModel = false;

#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	bIsUsingSimplifiedUserModel = IGDKRuntimeModule::Get().IsUsingSimplifiedUserModel();
#endif
	if (!bIsUsingSimplifiedUserModel && (GameInstance->GetCurrentGameContext() == EGameContext::EIntro || GameInstance->GetCurrentGameContext() == EGameContext::EWelcomeScreen))
	{
		return false;
	}
	int32 NumController = -1;
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	NumController = FXboxCommonPlatformInputDeviceMapper::Get().GetNumGameControllersForUser(UserId);
#endif
#if (PLATFORM_PS4 || PLATFORM_PS5)
	if (!IsConnected && !bDoneOnce)
	{	
		UNebulaFlowLocalPlayer* LocalPlayer = Cast<UNebulaFlowLocalPlayer>(GameInstance->FindLocalPlayerFromPlatformUserId(UserId));
		if (LocalPlayer && !LocalPlayer->IsWaitingForReconnectionConfirm())
		{
			NumController = 1;
		}
	}
#endif
	bool bIsValid = NumController < 1;
	if (bIsValid && Dialog && !Dialog->IsInViewport())
	{
		ShowDialogOnPause();
	}
	return bIsValid;
}

void UNebulaFlowControllerConnectionChangeTask::SuspensionImpementation()
{
	if (Dialog)
	{
		UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);
		GameInstance->GetTimerManager().ClearTimer(TimerHandler);
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		Dialog = nullptr;

		if (CanPause())
		{
			UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
		}
	}
}

void UNebulaFlowControllerConnectionChangeTask::OnTermination()
{
	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);
	if (Dialog)
	{
		
		GameInstance->GetTimerManager().ClearTimer(TimerHandler);
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
		if (Dialog->IsInViewport())
		{
			Dialog->RemoveFromParent();
		}
		Dialog = nullptr;
	}
	UNebulaFlowLocalPlayer* LocalPlayer = Cast<UNebulaFlowLocalPlayer>(GameInstance->FindLocalPlayerFromPlatformUserId(UserId));
	if (LocalPlayer)
	{
		if (CanPause())
		{
			UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
		}
		if (IsConnected)
		{
			if (LocalPlayer->IsWaitingForReconnectionConfirm())
			{
				LocalPlayer->UpdateConnectionStatus(IsConnected, false);
			}
		}
	}
}

void UNebulaFlowControllerConnectionChangeTask::ShowDialogOnPause()
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	

#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	if (bShowXboxSystemDialog)
	{

		FShowInputDeviceSelectorComplete OnShowInputDeviceSelectorComplete = ([this](const FShowInputDeviceSelectorParams&) {
		
			if(this!= nullptr && this->IsValidLowLevel())
			{ 
				//UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);  Not Needed?
				Terminate();
			}

		});

		if (!FXboxCommonPlatformApplicationMisc::ShowInputDeviceSelector(UserId, (OnShowInputDeviceSelectorComplete)))
		{
			UE_LOG(LogNebulaFlowUI, Warning, TEXT("Failed To Open Input Device Selector UI"));
		}
	}
	else
	
#endif
	{
		UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(GameInstance);
		UNebulaFlowUserSubSystem* UserSubSystem = UNebulaFlowSingletonFunctionLibrary::GetUserSubsystem(this);
		check(UserSubSystem);
		UNebulaFlowLocalPlayer* LocalPlayer = IsFirstPlayer ? UserSubSystem->GetPlayerOwner() : UserSubSystem->GetSecondaryPlayerOwner();
		check(LocalPlayer);

		TArray<ANebulaFlowPlayerController*> SyncControllers;
		if (IsFirstPlayer)
		{
			SyncControllers = { Cast<ANebulaFlowPlayerController>(LocalPlayer->PlayerController) };
		}

		Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, DialogId,
			[=, this](FString inResponse)
		{
			OnDialogResponse(inResponse);
		}
		, LocalPlayer->PlayerController, SyncControllers, TArray<FText>{ FText::FromString(LocalPlayer->GetNickname()) });
	}
}

void UNebulaFlowControllerConnectionChangeTask::ShowDialogWithoutPause()
{
	Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, DialogId,
		[=, this](FString inResponse)
	{
	});

	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);

	GameInstance->GetTimerManager().SetTimer(TimerHandler, 
	[=, this](){
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog); 
		Terminate();
		}, 
	DialogDuration, 
	false);
}

void UNebulaFlowControllerConnectionChangeTask::OnDialogResponse(const FString& inResponse)
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog);
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
	Terminate();
}

bool UNebulaFlowControllerConnectionChangeTask::CanPause()
{
	return PauseOnDisconnect;
}