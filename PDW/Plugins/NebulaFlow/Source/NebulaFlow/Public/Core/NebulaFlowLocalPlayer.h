// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Engine/LocalPlayer.h"
#include "NebulaFlowPersistentUser.h"
#include "Misc/CoreMiscDefines.h"
#include "NebulaFlowLocalPlayer.generated.h"


UCLASS()
class NEBULAFLOW_API UNebulaFlowLocalPlayer : public ULocalPlayer
{

	GENERATED_UCLASS_BODY()

public:

	virtual void PlayerAdded(class UGameViewportClient* InViewportClient, int32 InControllerID) override;
	virtual void PlayerAdded(class UGameViewportClient* InViewportClient, FPlatformUserId InUserID) override;

	virtual void PlayerRemoved() override;
	bool IsConnected() const { return bIsConnected; };
	bool IsWaitingForReconnectionConfirm() const { return bPendingReconnectionConfirm; };
	void OnReconnectionConfirmed();
	void UpdateConnectionStatus(bool bIsConnected, bool PendingReconnectionConfirm);

	virtual FString GetNickname() const;
	virtual FString GetDefaultNickname() const;

	virtual FString GetDefaultSlotName() const;

	virtual void SavePersistentUser(bool& OutResult, bool bSaveAsync= false);
	UNebulaFlowPersistentUser* GetPersistentUser() const;

	/** Initializes the PersistentUser */
	void LoadPersistentUser();

	int32 GetLocalPlayerIndex() const;


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<UNebulaFlowPersistentUser> PersistentUserClass;

	UFUNCTION()
	virtual void OnUserControllerConnectionChange(EInputDeviceConnectionState NewConnectionState, FPlatformUserId UserId, FInputDeviceId InputDeviceId);

	virtual void OnPersistentUserLoaded(const bool &bNewPersistentUser);

private:
	uint32 bIsConnected : 1;
	uint32 bPendingReconnectionConfirm : 1;

	/** Persistent user data stored between sessions (i.e. the user's savegame) */
	UPROPERTY()
	UNebulaFlowPersistentUser* PersistentUser;
};