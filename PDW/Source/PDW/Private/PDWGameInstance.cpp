// Fill out your copyright notice in the Description page of Project Settings.


#include "PDWGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/PDWEventSubsytem.h"
#include "UI/Core/PDWNavigationConfig.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "Data/PDWLocalPlayer.h"
#include "Managers/PDWActivitiesManager.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"

TObjectPtr<UPDWGameInstance> UPDWGameInstance::Get(UObject* WorldContextObject)
{
	return Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

//TODO remove
void UPDWGameInstance::LoadGame()
{
	UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	if (localPlayerOwner)
	{
		localPlayerOwner->LoadPersistentUser();
	}
}

void UPDWGameInstance::SaveGame()
{
	UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	if (localPlayerOwner)
	{
		bool bResult;
		localPlayerOwner->SavePersistentUser(bResult);
	}
}

void UPDWGameInstance::SetMultiPlayerOn(const bool bIsMultiPlayer)
{
	bIsMultiPlayerOn = bIsMultiPlayer;
	UPDWEventSubsytem::Get(this)->OnMultiplayerStateChangeEvent();
}

bool UPDWGameInstance::GetIsMultiPlayerOn() const
{
	return bIsMultiPlayerOn;
}

void UPDWGameInstance::SetCurrentSlot(const int SlotNumber)
{
	CurrentSaveSlotToLoad = SlotNumber;
}

int UPDWGameInstance::GetCurrentSlotToLoad() const
{
	return CurrentSaveSlotToLoad;
}


void UPDWGameInstance::Init()
{
	Super::Init();

	FlowActivitiesManagerInstance = Cast<UPDWActivitiesManager>(UNebulaFlowActivitiesManager::GetFlowActivitiesManager(this));

	TSharedRef<FCustomNavigationConfig> MyNavConfig = MakeShareable(new FCustomNavigationConfig());
    FSlateApplication::Get().SetNavigationConfig(MyNavConfig);
	
#if PLATFORM_XSX || PLATFORM_XBOXONE
	UNebulaFlowUserSubSystem* UserSubSystem = GetSubsystem<UNebulaFlowUserSubSystem>();
	check(UserSubSystem);
	UserSubSystem->OnControllerPairingChanged.AddUniqueDynamic(this, &UPDWGameInstance::HandleOnControllerPairingChanged);
#endif
	//FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &ThisClass::HandleAppReactivated);

	
	OnSwitchRemapJoyConClosed.AddUniqueDynamic(this, &ThisClass::OnRemapJoyConClosedEventCalled);
}


void UPDWGameInstance::HandleAppReactivated()
{
#if PLATFORM_XSX || PLATFORM_XBOXONE
	if (GetIsMultiPlayerOn())
	{
		ULocalPlayer* LocalPlayerOne = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->GetLocalPlayer();

		FPlatformUserId P1PlatformUserId = FPlatformUserId::CreateFromInternalId(-1);
		FInputDeviceId P1InputDeviceId = FInputDeviceId();
		const int32 P1ControllerId = LocalPlayerOne->GetControllerId();

		bool bMapped = IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(
			P1ControllerId,
			P1PlatformUserId,
			P1InputDeviceId
		);

		ULocalPlayer* LocalPlayerTwo = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this)->GetLocalPlayer();

		FPlatformUserId P2PlatformUserId = FPlatformUserId::CreateFromInternalId(-1);
		FInputDeviceId P2InputDeviceId = FInputDeviceId();
		const int32 P2ControllerId = LocalPlayerTwo->GetControllerId();

		bool bMapped2 = IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(
			P2ControllerId,
			P2PlatformUserId,
			P2InputDeviceId
		);
		bool bSameUser = false;

		// This method returns the platform user associated with the given input device
		FPlatformUserId Id1 = IPlatformInputDeviceMapper::Get().GetUserForInputDevice(P1InputDeviceId);
		FPlatformUserId Id2 = IPlatformInputDeviceMapper::Get().GetUserForInputDevice(P2InputDeviceId);
		bSameUser = Id1.GetInternalId() == Id2.GetInternalId();

		bool bRemove = (LocalPlayerOne && LocalPlayerTwo) && (P1PlatformUserId == P2PlatformUserId) && bSameUser;
		if (bRemove)
		{
			APDWGameplayGameMode* GM = UPDWGameplayFunctionLibrary::GetPDWGameplayGameMode(this);
			if (GM)
			{
				GM->RemovePlayer2();
			}
		}
	}
#endif
}

void UPDWGameInstance::HandleOnControllerPairingChanged()
{
	APDWGameplayGameMode* GM = UPDWGameplayFunctionLibrary::GetPDWGameplayGameMode(this);
	if (GM)
	{
		GM->RemovePlayer2();
	}
}

void UPDWGameInstance::HideLoadingScreen(UWorld* InLoadedWorld)
{
	Super::HideLoadingScreen(InLoadedWorld);
	if (InLoadedWorld->GetName() == "WorldMap")
	{
		ShowLoadingScreen("");
	}
}

void UPDWGameInstance::OnRemapJoyConClosedEventCalled(bool bSingleJoyCon)
{
	// Make sure we enable/disable joycon based on real value
	UGameOptionsFunctionLibrary::SetSwapJoystickEnabled(this, UGameOptionsFunctionLibrary::bSwapJoystickEnabled);
}

void UPDWGameInstance::OnGameReady()
{
	Super::HideLoadingScreen(GetWorld());
}

void UPDWGameInstance::InitActivityManager()
{
	if (FlowActivitiesManagerInstance)
	{
		ULocalPlayer* Player = Cast<UPDWLocalPlayer>(GetLocalPlayerByIndex(0));
		FlowActivitiesManagerInstance->Init(Player);
	}
}
