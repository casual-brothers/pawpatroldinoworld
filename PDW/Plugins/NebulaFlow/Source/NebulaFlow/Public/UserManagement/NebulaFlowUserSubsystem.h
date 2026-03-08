#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Online/CoreOnline.h"
#include "UObject/SoftObjectPtr.h"
#include "NebulaFlowUserTaskConfiguration.h"
#include "Engine/DataTable.h"
#include "OnlineSubsystemTypes.h"
#include "Misc/Optional.h"
#include "NebulaFlowUserSubsystem.generated.h"

class UNebulaFlowLocalPlayer;
class UNebulaFlowDefinePlayerOwnerTask;
class UNebulaFlowHandleLoginTask;
class UNebulaFlowMissingOwningPlayerTask;
class UNebulaFlowControllerConnectionChangeTask;
class FTickableObjectBase;
class UNebulaFlowDialog;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserLoggedId, int32, ControllerIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnControllerPairingChanged2);

struct FFlowUserLoginChanged
{
public:
	int32 GameUserIndex;
	ELoginStatus::Type PreviousLoginStatus;
	ELoginStatus::Type LoginStatus;
	//const FUniqueNetId& UserId;

	FFlowUserLoginChanged(int32 GameUserIndexIN, ELoginStatus::Type PreviousLoginStatusIN, ELoginStatus::Type LoginStatusIN/*, const FUniqueNetId& UserIdIN*/)
		: GameUserIndex(GameUserIndexIN)
		, PreviousLoginStatus(PreviousLoginStatusIN)
		, LoginStatus(LoginStatusIN)
		//, UserId(UserIdIN)
	{
	}
};

UCLASS(Config=Engine)
class NEBULAFLOW_API UNebulaFlowUserSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void RequireUserLogin(int32 UserIndex);
	virtual void RequireSecondaryPlayerUserLogin(int32 UserIndex);
	virtual void RequireChangeUser(int32 UserIndex);
	virtual void RedefinePlayerOwner(int32 UserIndex);
	const FNebulaFlowUserTaskConfiguration* FindConfigurationForUserTaskType(const EUserTaskType& userTaskType) const;
	UNebulaFlowLocalPlayer* GetPlayerOwner();
	void SetPlayerOwner(UNebulaFlowLocalPlayer* playerOwner);

	UNebulaFlowLocalPlayer* GetSecondaryPlayerOwner();
	void SetSecondaryPlayerOwner(UNebulaFlowLocalPlayer* SecondPlayer);

	bool IsLocalPlayerOnline(class ULocalPlayer* LocalPlayer);

	void Tick(float DeltaTime);

	UPROPERTY()
	bool InConstrainedMode = false;

protected:

	void HandleUserLoginStatusChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId);
	void HandleControllerPairingChanged(int32 LocalUserNum, FControllerPairingChangedUserInfo PreviousUser, FControllerPairingChangedUserInfo NewUser);	
	void HandleControllerConnectionChanged(EInputDeviceConnectionState NewConnectionState, FPlatformUserId UserId, FInputDeviceId InputDeviceId);
	void HandleOnLoginComplete(int32 GameUserIndex, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void HandleOnLoginChange(int32 LocalUserNum);
	void HandleAppSuspend();
	void HandleAppResume();
	void HandleDeviceConnected(void* device);
	void HandleSaveFailed();
	void HandleOnConstrained();
	void HandleOnUnconstrained();

	void HandleOnConnectionChange(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus);

	UPROPERTY(BlueprintReadWrite)
		UNebulaFlowDialog* RemapDialogOnScreen {};

	UPROPERTY(BlueprintReadWrite)
		UNebulaFlowDialog* ReturnToMainMenuDialogOnScreen {};

private:

	void InitializeConfiguration();
	void InitializeUserLoginStatusChangedHandler(const IOnlineIdentityPtr& identityInterface);
	void InitializeControllerPairingChangedHandler(const IOnlineIdentityPtr& identityInterface);
	void InitializeControllerConnectionChangedHandler();
	void InitializeOnLoginCompleteHandler(const IOnlineIdentityPtr& identityInterface);
	void InitializeOnLoginChangedHandler(const IOnlineIdentityPtr& identityInterface);
	void InitializeOnAppSuspendHandler();
	void InitializeOnAppResumeHandler();
	void InitializeOnDeviceConnectedHandler();
	void InitializeOnSaveFailedHandler();
	void InitializeOnConstrained();
	void InitializeOnUnconstrained();

	void InitializeOnConnectionChangedHandler();

public:

	UPROPERTY()
	FOnUserLoggedId OnUserLoggedInDelegate;

	UPROPERTY()
	FOnControllerPairingChanged2 OnControllerPairingChanged;

public:

	/** Local player login status when the system is suspended */
	TArray<ELoginStatus::Type> LocalPlayerOnlineStatus;

	TOptional<FFlowUserLoginChanged> LogginOutInProgress;

	FTimerHandle LogginOutInProgresseTimerHandle;

private:

	UPROPERTY(Config)
	TSoftObjectPtr<class UDataTable> ConfigurationPath;

	UPROPERTY()
	UNebulaFlowLocalPlayer* CurrentPlayerOwner = nullptr;

	UPROPERTY()
	UNebulaFlowLocalPlayer* SecondaryPlayerOwner = nullptr;

	UPROPERTY()
	TMap<EUserTaskType, FNebulaFlowUserTaskConfiguration> UserTaskConfigurationMap;

#if PLATFORM_SWITCH

public:

	void OnConnectedJoyconsUpdate(const int JoyconsConnected);

	int GetSwitchConnectedControllers() const { return SwitchConnectedControllers; }
	void SetSwitchConnectedControllers(int val) { SwitchConnectedControllers = val; }

	int GetInGamePlayers() const { return InGamePlayers; }
	void SetInGamePlayers(int val) { InGamePlayers = val; }

	bool GetSingleJoyconEnabled() const { return SingleJoyconEnabled; }
	
	void SetSingleJoyconEnabled(bool val) { SingleJoyconEnabled = val; }

	void OnSwitchAppletShown();

protected:

	void HandleSwitchRemapRequest();

	void HandleSwitchShowingApplet();
	
	volatile bool CreateTask = false;

private:

	int SwitchConnectedControllers = 1;

	int InGamePlayers = 1;

	bool SingleJoyconEnabled = true;

	bool LessJoyconsThenPlayers = false;

	bool bShowingAppletUpdate = false; //The showing applet will happen in another thread so we save a bool and we create the corresponding task the next frame
	
	FName MissingControllersID = FName("NOT_ENOUGH_CONTROLLERS");

	FName AreYouSureID = FName("ARE_YOU_SURE");

#endif

	/////////////////////////////
};