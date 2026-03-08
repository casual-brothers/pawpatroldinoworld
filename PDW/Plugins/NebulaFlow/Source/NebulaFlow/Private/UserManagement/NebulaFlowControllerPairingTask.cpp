#include "UserManagement/NebulaFlowControllerPairingTask.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"

void UNebulaFlowControllerPairingTask::InitializeTask(int32 localUserNum, const FControllerPairingChangedUserInfo& previousUser, const FControllerPairingChangedUserInfo& newUser)
{
	LocalUserNum = localUserNum;
	PreviousUserId = GetControllerIdFromControllerPairingChangedUserInfo(previousUser);
	NextUserId = GetControllerIdFromControllerPairingChangedUserInfo(newUser);	
	PreviousPlatformUserId = GetPlatformUserIdFromControllerPairingChangedUserInfo(previousUser);
	NextPlatformUserId = GetPlatformUserIdFromControllerPairingChangedUserInfo(newUser);
}

int32 UNebulaFlowControllerPairingTask::GetControllerIdFromControllerPairingChangedUserInfo(const FControllerPairingChangedUserInfo& user) const
{
	UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(this);
	check(gameInstance);
	ULocalPlayer* localPlayer = gameInstance->FindLocalPlayerFromUniqueNetId(user.User);
	if (localPlayer)
	{
		return localPlayer->GetControllerId();
	}
	return INDEX_NONE;
}

FPlatformUserId UNebulaFlowControllerPairingTask::GetPlatformUserIdFromControllerPairingChangedUserInfo(const FControllerPairingChangedUserInfo& user) const
{
	UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(this);
	check(gameInstance);
	ULocalPlayer* localPlayer = gameInstance->FindLocalPlayerFromUniqueNetId(user.User);
	if (localPlayer)
	{
		return localPlayer->GetPlatformUserId();
	}
	return FPlatformUserId();
}

