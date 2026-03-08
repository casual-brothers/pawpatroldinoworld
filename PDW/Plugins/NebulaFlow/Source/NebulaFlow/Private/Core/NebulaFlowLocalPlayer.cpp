#include "Core/NebulaFlowLocalPlayer.h"
#include "Core/NebulaFlowGameInstance.h"
#include "OnlineSubsystem.h"
#include "Online.h"
#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CoreMiscDefines.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Engine/GameViewportClient.h"

#define MAX_PLAYER_NAME_LENGTH 32

UNebulaFlowLocalPlayer::UNebulaFlowLocalPlayer(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsConnected = true;
	bPendingReconnectionConfirm = false;

	PersistentUserClass = UNebulaFlowPersistentUser::StaticClass();
}

void UNebulaFlowLocalPlayer::PlayerAdded(UGameViewportClient* InViewportClient, int32 InControllerID)
{
	Super::PlayerAdded(InViewportClient, InControllerID);
	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this, &UNebulaFlowLocalPlayer::OnUserControllerConnectionChange);
}

void UNebulaFlowLocalPlayer::PlayerAdded(UGameViewportClient* InViewportClient, FPlatformUserId InUserID)
{
	Super::PlayerAdded(InViewportClient, InUserID);
	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this, &UNebulaFlowLocalPlayer::OnUserControllerConnectionChange);
}

void UNebulaFlowLocalPlayer::PlayerRemoved()
{
	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().RemoveAll(this);
	Super::PlayerRemoved();
}

void UNebulaFlowLocalPlayer::OnReconnectionConfirmed()
{
	bPendingReconnectionConfirm = false;
}

FString UNebulaFlowLocalPlayer::GetNickname() const
{
	const auto OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
	if (OnlineSub)
	{
		if (!OnlineSub->GetAppId().IsEmpty())
		{
			FString UserNickName = Super::GetNickname();

			if (UserNickName.Len() > MAX_PLAYER_NAME_LENGTH)
			{
				UserNickName = UserNickName.Left(MAX_PLAYER_NAME_LENGTH) + "...";
			}

			bool bReplace = (UserNickName.Len() == 0);

			// Check for duplicate nicknames...and prevent reentry
			static bool bReentry = false;
			if (!bReentry)
			{
				bReentry = true;
				UNebulaFlowGameInstance* GameInstance = GetWorld() != nullptr ? Cast<UNebulaFlowGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
				if (GameInstance)
				{
					// Check all the names that occur before ours that are the same
					const TArray<ULocalPlayer*>& LocalPlayers = GameInstance->GetLocalPlayers();
					for (int i = 0; i < LocalPlayers.Num(); ++i)
					{
						const ULocalPlayer* LocalPlayer = LocalPlayers[i];
						if (this == LocalPlayer)
						{
							break;
						}

						if (UserNickName == LocalPlayer->GetNickname())
						{
							bReplace = true;
							break;
						}
					}
				}
				bReentry = false;
			}

			if (bReplace)
			{
				UserNickName = GetDefaultNickname();
			}

			return UserNickName;
		}
	}

	return GetDefaultNickname();
}

FString UNebulaFlowLocalPlayer::GetDefaultNickname() const
{
	return FString::Printf(TEXT("%s %i"), *FString("Player"), GetLocalPlayerIndex() + 1);
}

FString UNebulaFlowLocalPlayer::GetDefaultSlotName() const
{
	return FString::Printf(TEXT("FlowSaveGame"));
}

void UNebulaFlowLocalPlayer::SavePersistentUser(bool& OutResult, bool bSaveAsync)
{
	UNebulaFlowGameInstance* GameInstance = GetWorld() != nullptr ? Cast<UNebulaFlowGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
	if (GameInstance)
	{
		if (!GameInstance->IsSaveSystemEnabled())
		{
			return;
		}

		if (PersistentUser != nullptr)
		{
			FUniqueNetIdRepl UserId = GetPreferredUniqueNetId();

			if (UserId.IsValid())
			{
				GameInstance->ShowSaveIcon();
				if(!bSaveAsync)
				{ 
					PersistentUser->SetDirty();
				}
				PersistentUser->SaveIfDirty(OutResult,bSaveAsync);
			}
		}
	}
}

UNebulaFlowPersistentUser* UNebulaFlowLocalPlayer::GetPersistentUser() const
{
	return PersistentUser;
}

int32 UNebulaFlowLocalPlayer::GetLocalPlayerIndex() const
{

	UNebulaFlowGameInstance* GameInstance = GetWorld() != nullptr ? Cast<UNebulaFlowGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
	if (GameInstance)
	{
		return GameInstance->FindLocalPlayerIndexByControllerId(GetControllerId());
	}

	return INDEX_NONE;

}

void UNebulaFlowLocalPlayer::UpdateConnectionStatus(bool IsConnected, bool PendingReconnectionConfirm)
{
	bIsConnected = IsConnected;	
	bPendingReconnectionConfirm = PendingReconnectionConfirm;
}

void UNebulaFlowLocalPlayer::OnUserControllerConnectionChange(EInputDeviceConnectionState NewConnectionState, FPlatformUserId UserId, FInputDeviceId InputDeviceId)
{
	if (UserId == GetPlatformUserId())
	{
		bIsConnected = NewConnectionState == EInputDeviceConnectionState::Connected;
		if (!bIsConnected)
		{
			bPendingReconnectionConfirm = true;
		}
		else if (bIsConnected && bPendingReconnectionConfirm)
		{
			bPendingReconnectionConfirm = false;
		}
	}
}

void UNebulaFlowLocalPlayer::OnPersistentUserLoaded(const bool &bNewPersistentUser)
{
	UNebulaFlowGameInstance* GameInstance = GetWorld() != nullptr ? Cast<UNebulaFlowGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
	
	//Input Assignment on PostLoad, To Reinsert when support is Ready ->Luca Nucera

	if (GameInstance)
	{
		if (bNewPersistentUser)
		{
			UNebulaFlowInputFunctionLibrary::ApplyDefaultInputs(GameInstance);
		}
		else
		{
			UNebulaFlowInputFunctionLibrary::ApplyAssignmentToInputs(GameInstance, true);
			UNebulaFlowInputFunctionLibrary::ApplyAssignmentToInputs(GameInstance, false);
		}
	}

}

void UNebulaFlowLocalPlayer::LoadPersistentUser()
{

	FString Nickname = GetNickname();
	FString SaveGameName = GetDefaultSlotName();

	/*
#if PLATFORM_SWITCH
	// on Switch, the displayable nickname can change, so we can't use it as a save ID (explicitly stated in docs, so changing for pre-cert)
	FPlatformMisc::GetUniqueStringNameForControllerId(GetControllerId(), SaveGameName);
#endif
	*/

	// if we changed controllerid / user, then we need to load the appropriate persistent user.
	//if (PersistentUser != nullptr && (GetControllerId() != PersistentUser->GetUserIndex() || Nickname != PersistentUser->GetUserName()))
	//{
		
		PersistentUser = nullptr;
	//}

	if (PersistentUser == nullptr)
	{
		// Use the platform id here to be resilient in the face of controller swapping and similar situations.

		FPlatformUserId PlatformId = FPlatformMisc::GetPlatformUserForUserIndex(GetControllerId());
		UNebulaFlowGameInstance* GameInstance = GetWorld() != nullptr ? Cast<UNebulaFlowGameInstance>(GetWorld()->GetGameInstance()) : nullptr;

		//auto Identity = Online::GetIdentityInterface();
		//if (Identity.IsValid() && GetPreferredUniqueNetId().IsValid())
		//{
		//	PlatformId = Identity->GetPlatformUserIdFromUniqueNetId(*GetPreferredUniqueNetId());
		//}
#if WITH_EDITOR
	PlatformId = FPlatformMisc::GetPlatformUserForUserIndex(0);
#endif
		if (GameInstance)
		{
			GameInstance->ShowSaveIcon();
		}

//#if PLATFORM_XSX
	//	UKismetSystemLibrary::ExecuteConsoleCommand(this, FString::Printf(TEXT("GDK.UseXDKCompatibleSave 1")));
//#endif

		bool bNewPersistentUser;
		
		PersistentUser = UNebulaFlowPersistentUser::LoadPersistentUser(GameInstance, SaveGameName, Nickname, PlatformId, PersistentUserClass, bNewPersistentUser);

		OnPersistentUserLoaded(bNewPersistentUser);

		GameInstance->OnPersistentUserLoaded(GetControllerId());
	}
}

