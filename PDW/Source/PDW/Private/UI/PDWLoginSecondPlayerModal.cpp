// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PDWLoginSecondPlayerModal.h"
#include "Kismet/GameplayStatics.h"
#include "PDWGameInstance.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "UObject/UObjectGlobals.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Data/PDWGameSettings.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"

void UPDWLoginSecondPlayerModal::InitDialog(APlayerController* InControllerOwner, FName DialogID, TFunction<void(FString) > InCallBack, const TArray<ANebulaFlowPlayerController*>& SyncControllers /*=*/, const TArray<FText>& Params /*=*/, UTexture2D* Image /*= nullptr*/, bool ibForceFocus /*= false*/, TObjectPtr<UInputMappingContext> InputMappingContext /*= */)
{
	//Force focus for listen second player input
	Super::InitDialog(InControllerOwner, DialogID, InCallBack, SyncControllers, Params, Image, false, InputMappingContext);
	
#if (PLATFORM_SWITCH)
	FCoreUObjectDelegates::OnRemapJoyconsUIClosed.AddUObject(this, &ThisClass::CheckConfigurationJoycon);
	//Show Applet
	UNebulaFlowConsoleFunctionLibrary::SetSwitchMaxControllers(2);
	UNebulaFlowConsoleFunctionLibrary::ShowRemapJoycons();
#else
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	UNebulaFlowUserSubSystem* UserSubsystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	if (UserSubsystem)
	{
		UserSubsystem->OnUserLoggedInDelegate.AddUniqueDynamic(this, &ThisClass::ManageCreatePlayer2);
	}
#endif
}

void UPDWLoginSecondPlayerModal::NativeDestruct()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	UNebulaFlowUserSubSystem* UserSubsystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	if (UserSubsystem)
	{
		UserSubsystem->OnUserLoggedInDelegate.RemoveDynamic(this, &ThisClass::ManageCreatePlayer2);
	}

#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnRemapJoyconsUIClosed.RemoveAll(this);
#endif
	
	Super::NativeDestruct();
}

void UPDWLoginSecondPlayerModal::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (GameInstance)
	{
		const auto& LocalPlayers = GameInstance->GetLocalPlayers();
		FSlateApplication::Get().ForEachUser([LocalPlayers, this](FSlateUser& User){
			for (const auto* LocalPlayer : LocalPlayers)
			{
				if (LocalPlayer && LocalPlayer->GetSlateUser())
				{

					bool isCacheValid = true;
#if  ((!PLATFORM_WINDOWS || PLATFORM_WINGDK) && !PLATFORM_SWITCH)
					isCacheValid = LocalPlayer->CachedUniqueNetId.IsValid() && LocalPlayer->CachedUniqueNetId->IsValid();
#endif
					isCacheValid = true;
					if (LocalPlayer->GetSlateUser()->GetUserIndex() == User.GetUserIndex() && isCacheValid)
					{
						return;
					}
				}
			}
			FSlateApplication::Get().SetUserFocus(User.GetUserIndex(), this->TakeWidget(), EFocusCause::SetDirectly);
		});
	}

#if PLATFORM_SWITCH
	if (bCheckConfigurationJoycon)
	{
		bCheckConfigurationJoycon = false;
		if (bResultApplet && UNebulaFlowConsoleFunctionLibrary::IsJoyconConnected(1))
		{
			ManageCreatePlayer2(1);
		}
		else
		{
			UNebulaFlowConsoleFunctionLibrary::SetSwitchMaxControllers(1);
			SendDialogResponse(UPDWGameSettings::GetActionJoinPlayer2Fail().ToString());
		}
	}
#endif
}

FReply UPDWLoginSecondPlayerModal::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
#if PLATFORM_SWITCH
	return FReply::Unhandled();
#else
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	int32 EventInputId = InKeyEvent.GetInputDeviceId().GetId();

	int32 EventControllerId = InKeyEvent.GetUserIndex();
	int32 EventPlatformUserId = InKeyEvent.GetPlatformUserId();
	
	ULocalPlayer* LocalPlayerOne = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->GetLocalPlayer();

	FPlatformUserId P1PlatformUserId;
    FInputDeviceId P1InputDeviceId;
	const int32 P1ControllerId = LocalPlayerOne->GetControllerId();

    bool bMapped = IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(
        P1ControllerId,
        P1PlatformUserId,
        P1InputDeviceId
    );

	if(EventInputId == P1InputDeviceId.GetId())
	{
		return FReply::Unhandled();
	}
	
	UPDWGameInstance* GM = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GM->GetIsMultiPlayerOn())
	{
		return FReply::Unhandled();
	}

#if	((PLATFORM_WINDOWS || PLATFORM_XSX || PLATFORM_XBOXONE) && !PLATFORM_WINGDK)
	if (EventPlatformUserId == P1PlatformUserId)
	{
		return FReply::Unhandled();
	}
	else
	{
		ManageCreatePlayer2(EventControllerId);
		return FReply::Handled();
	}
#else
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	check(GameInstance);
	UNebulaFlowUserSubSystem* UserSubsystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	if (UserSubsystem)
	{
		UserSubsystem->RequireSecondaryPlayerUserLogin(EventControllerId);
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
#endif
#endif
}

void UPDWLoginSecondPlayerModal::ManageCreatePlayer2(int32 UserIndex)
{
	APDWGameplayGameMode* GM = UPDWGameplayFunctionLibrary::GetPDWGameplayGameMode(this);
	if (GM)
	{
		GM->CreatePlayer2(UserIndex);
	}
	SendDialogResponse(UPDWGameSettings::GetActionJoinPlayer2Succes().ToString(), GM->GetPlayerControllerOne());
}

void UPDWLoginSecondPlayerModal::CheckConfigurationJoycon(bool IsRemap)
{
	bCheckConfigurationJoycon = true;
	bResultApplet = IsRemap;
}