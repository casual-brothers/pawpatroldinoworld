#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "TaskManagement/NebulaFlowTaskSubsystem.h"
#include "UserManagement/NebulaFlowHandleLoginTask.h"
#include "UserManagement/NebulaFlowControllerPairingTask.h"
#include "UserManagement/NebulaFlowUserTaskConfiguration.h"
#include "Engine/DataTable.h"
#include "Containers/Set.h"
#include "UserManagement/NebulaFlowMultipleControllerTask.h"
#include "UserManagement/NebulaFlowMissingOwningPlayerTask.h"
#include "UserManagement/NebulaFlowControllerConnectionChangeTask.h"
#include "Online/CoreOnline.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UObject/Object.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "NebulaFlow.h"
#include "UserManagement/NebulaFlowDefinePlayerOwnerTask.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Engine/LocalPlayer.h"
#include "UserManagement/NebulaFlowAppResumeTask.h"
#include "Stats/Stats.h"
#include "Stats/Stats2.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "UserManagement/NebulaFlowRemapJoyconsTask.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/NebulaFlowDialog.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowFSMManager.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "UserManagement/NebulaFlowDeviceConnectedTask.h"
#include "UserManagement/NebulaFlowDefineSecondaryPlayerTask.h"
#include "UserManagement/NebulaFlowChangeUserTask.h"
#include "Logging/LogMacros.h"
#include "UserManagement/NebulaFlowMissingSecondaryPlayerTask.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "UserManagement/NebulaFlowSaveFailedTask.h"
#include "UserManagement/NebulaFlowRedefinePlayerOwnerTask.h"
//#TODO_NEBULA
//#include "UserManagement/NebulaFlowAppletShownTask.h"
#include "Engine/GameInstance.h"
#include "UserManagement/NebulaFlowAppletShownTask.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "Core/NebulaFlowPlayerController.h"

// THIS CLASS IS DESIGNED TO BIND TO ANY DELEGATES FIRED BY THE CONSOLES THAT NEEDS TO BE MANAGED
// AND CREATES TASKS THAT WILL BE MANAGED BY TASKS SUBSYSTEM


void UNebulaFlowUserSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LocalPlayerOnlineStatus.InsertDefaulted(0, MAX_LOCAL_PLAYERS);
	InitializeConfiguration();
	IOnlineIdentityPtr onlineIdentity = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	InitializeUserLoginStatusChangedHandler(onlineIdentity);
	InitializeControllerPairingChangedHandler(onlineIdentity);
	InitializeControllerConnectionChangedHandler();
	InitializeOnAppSuspendHandler();
	InitializeOnAppResumeHandler();
	InitializeOnDeviceConnectedHandler();
	InitializeOnSaveFailedHandler();
	InitializeOnConstrained();
	InitializeOnUnconstrained();

	InitializeOnConnectionChangedHandler();

#if PLATFORM_SWITCH

	//we initialize the array with the starting switch configuration
	FCoreUObjectDelegates::OnCurrentConnectedJoyconsUpdate.AddUObject(this, &UNebulaFlowUserSubSystem::OnConnectedJoyconsUpdate);
	FCoreUObjectDelegates::OnRemapJoyconsUIOpen.AddUObject(this, &UNebulaFlowUserSubSystem::OnSwitchAppletShown);

#endif
}

void UNebulaFlowUserSubSystem::Deinitialize()
{
#if PLATFORM_SWITCH

	//we initialize the array with the starting switch configuration
	FCoreUObjectDelegates::OnCurrentConnectedJoyconsUpdate.RemoveAll(this);
	FCoreUObjectDelegates::OnRemapJoyconsUIOpen.RemoveAll(this);

#endif
	Super::Deinitialize();
}

void UNebulaFlowUserSubSystem::RequireUserLogin(int32 UserIndex)
{
	UE_LOG(LogClass, Warning, TEXT("Task: RequireUserLogin"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowDefinePlayerOwnerTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EDefinePlayerOwner), UserIndex);
}

void UNebulaFlowUserSubSystem::RequireSecondaryPlayerUserLogin(int32 UserIndex)
{
	UE_LOG(LogClass, Warning, TEXT("Task: RequireSecondaryPlayerUserLogin"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowDefineSecondaryPlayerTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EDefineSecondaryPlayer), UserIndex);
}

void UNebulaFlowUserSubSystem::RequireChangeUser(int32 UserIndex)
{
	UE_LOG(LogClass, Warning, TEXT("Task: RequireChangeUser"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowChangeUserTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EChangeUser), UserIndex);
}

void UNebulaFlowUserSubSystem::RedefinePlayerOwner(int32 UserIndex)
{
	UE_LOG(LogClass, Warning, TEXT("Task: RedefinePlayerOwner"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowRedefinePlayerOwnerTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::ERedefinePlayerOwner), UserIndex);
}

const FNebulaFlowUserTaskConfiguration* UNebulaFlowUserSubSystem::FindConfigurationForUserTaskType(const EUserTaskType& userTaskType) const
{
	const auto* userTaskConfiguration = UserTaskConfigurationMap.Find(userTaskType);
	check(userTaskConfiguration);
	return userTaskConfiguration;
}

UNebulaFlowLocalPlayer* UNebulaFlowUserSubSystem::GetPlayerOwner()
{
	//This Function must be overridden in game specific logics; can contain logics for setting owning player on platform basis

#if (PLATFORM_XBOXONE || PLATFORM_XSX || PLATFORM_WINGDK)

	if (CurrentPlayerOwner)
	{
		return CurrentPlayerOwner;
	}

#endif

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (GameInstance && GameInstance->GetFirstGamePlayer())
	{
		UNebulaFlowLocalPlayer* LocalPlayer = Cast<UNebulaFlowLocalPlayer>(GameInstance->GetFirstGamePlayer());
		if (!LocalPlayer)
		{
			UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Local Player MUST BE a subclass of UNebulaFlowLocalPlayer"));
		}
		return LocalPlayer;
	}
	return nullptr;
}

void UNebulaFlowUserSubSystem::SetPlayerOwner(UNebulaFlowLocalPlayer* playerOwner)
{
	CurrentPlayerOwner = playerOwner;
}

UNebulaFlowLocalPlayer* UNebulaFlowUserSubSystem::GetSecondaryPlayerOwner()
{
	if (SecondaryPlayerOwner)
	{
		return SecondaryPlayerOwner;
	}

	return nullptr;
}

void UNebulaFlowUserSubSystem::SetSecondaryPlayerOwner(UNebulaFlowLocalPlayer* SecondPlayer)
{
	SecondaryPlayerOwner = SecondPlayer;
}

bool UNebulaFlowUserSubSystem::IsLocalPlayerOnline(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer == NULL)
	{
		return false;
	}
	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	if (IdentityInterface.IsValid())
	{
		auto UniqueId = LocalPlayer->GetCachedUniqueNetId();
		if (UniqueId.IsValid())
		{
			const auto LoginStatus = IdentityInterface->GetLoginStatus(*UniqueId);
			if (LoginStatus == ELoginStatus::LoggedIn)
			{
				return true;
			}
#if PLATFORM_PS4 || PLATFORM_PS5
			if (LoginStatus == ELoginStatus::UsingLocalProfile)
			{
				return true;
			}
#endif
		}
	}

	return false;
}

void UNebulaFlowUserSubSystem::Tick(float DeltaTime)
{
#if PLATFORM_SWITCH

	//#TODO_UI to also correct this point always for the double copy of the subsystem

	if (bShowingAppletUpdate == true)
	{
		bShowingAppletUpdate = false;
		HandleSwitchShowingApplet();
	}

	if (SingleJoyconEnabled == false && UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0))
	{
		UNebulaFlowConsoleFunctionLibrary::ShowRemapJoycons();
	}

	if (CreateTask)
	{
		HandleSwitchRemapRequest();
	}

#endif
}

void UNebulaFlowUserSubSystem::HandleUserLoginStatusChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId)
{
	UE_LOG(LogClass, Warning, TEXT("Task: HandleUserLoginStatusChanged"));
	EUserTaskType TaskType = EUserTaskType::EUserLoginChangedSecondaryPlayer;
	auto* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		ULocalPlayer* LocalPlayer = GameInstance->FindLocalPlayerFromControllerId(GameUserIndex);
		if (LocalPlayer && LocalPlayer->IsPrimaryPlayer())
		{
			TaskType = EUserTaskType::EUserLoginChangedPrimaryPlayer;
		}
	}
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowHandleLoginTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(TaskType), GameUserIndex, PreviousLoginStatus, LoginStatus, UserId);
}

void UNebulaFlowUserSubSystem::HandleControllerPairingChanged(int32 LocalUserNum, FControllerPairingChangedUserInfo PreviousUser, FControllerPairingChangedUserInfo NewUser)
{
#if !PLATFORM_WINDOWS
	if (PreviousUser.ControllersRemaining == 0)
	{
		UE_LOG(LogClass, Warning, TEXT("Task: HandleControllerPairingChanged"));
		UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowMissingOwningPlayerTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EMissingPlayerOwner), LocalUserNum, PreviousUser, NewUser);
		UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowMissingSecondaryPlayerTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EMissingSecondaryPlayer), LocalUserNum, PreviousUser, NewUser);
	}
	if (NewUser.ControllersRemaining > 1)
	{
		UE_LOG(LogClass, Warning, TEXT("Task: UNebulaFlowMultipleControllerTask"));
		UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowMultipleControllerTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EMultipleController), LocalUserNum, PreviousUser, NewUser);
	}
	OnControllerPairingChanged.Broadcast();	
#endif
}

void UNebulaFlowUserSubSystem::HandleControllerConnectionChanged(EInputDeviceConnectionState NewConnectionState, FPlatformUserId UserId, FInputDeviceId InputDeviceId)
{
#if !PLATFORM_WINDOWS
	bool bIsConnected = NewConnectionState == EInputDeviceConnectionState::Connected;
	EUserTaskType TaskType = bIsConnected ? EUserTaskType::EControllerConnectionConnectedSecondaryPlayer : EUserTaskType::EControllerConnectionDisconnectedSecondaryPlayer;
	//UE_LOG(LogClass, Warning, TEXT("Task: HandleControllerConnectionChanged"));
	auto* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		ULocalPlayer* LocalPlayer = GameInstance->FindLocalPlayerFromPlatformUserId(UserId);
		if (LocalPlayer && LocalPlayer->IsPrimaryPlayer())
		{
			TaskType = bIsConnected ? EUserTaskType::EControllerConnectionConnectedPrimaryPlayer : EUserTaskType::EControllerConnectionDisconnectedPrimaryPlayer;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Task: %s"), *UEnum::GetValueAsName(TaskType).ToString());

	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowControllerConnectionChangeTask>(this, GameInstance, UserTaskConfigurationMap.Find(TaskType), bIsConnected, UserId, InputDeviceId.GetId());
#endif
}

void UNebulaFlowUserSubSystem::HandleOnLoginComplete(int32 GameUserIndex, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
}

void UNebulaFlowUserSubSystem::HandleOnLoginChange(int32 LocalUserNum)
{
}

void UNebulaFlowUserSubSystem::HandleAppSuspend()
{
	auto* taskSubsystem = GetGameInstance()->GetSubsystem<UNebulaFlowTaskSubsystem>();
	check(taskSubsystem);
	UE_LOG(LogClass, Warning, TEXT("Task: HandleAppSuspend"));
	taskSubsystem->SuspendTasks();
}

void UNebulaFlowUserSubSystem::HandleAppResume()
{
	UE_LOG(LogClass, Warning, TEXT("Task: HandleAppResume"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowAppResumeTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EAppResume));}

#if PLATFORM_SWITCH

void UNebulaFlowUserSubSystem::HandleSwitchRemapRequest()
{
	UE_LOG(LogClass, Warning, TEXT("Task: HandleSwitchRemapRequest"));
	auto* gameInstance = UGameplayStatics::GetGameInstance(this);

// 	if (gameInstance)
// 	{
		//#TODO_UI we check the gameinstance here , otherwise if we create a task, but the gameinstance is nullptr the garbage collector crashes
		UNebulaFlowTaskSubsystem* TaskSubsystem = gameInstance->GetSubsystem<UNebulaFlowTaskSubsystem>();
		if (TaskSubsystem)
		{
			TArray<UNebulaFlowTaskInterface *> TaskList = TaskSubsystem->GetTaskList();
			for (auto TaskInterface : TaskList)
			{
				if (TaskInterface->IsA(UNebulaFlowRemapJoyconsTask::StaticClass()))
				{
					return;
				}
			}
			UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowRemapJoyconsTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::ESwitchRemapJoycon), GetInGamePlayers());
		}
	//}
}

void UNebulaFlowUserSubSystem::HandleSwitchShowingApplet()
{
	//UE_LOG(LogClass, Warning, TEXT("Task: HandleAppletShown"));
	auto* gameInstance = UGameplayStatics::GetGameInstance(this);

	if (gameInstance)
	{
		//#TODO_UI we check the gameinstance here , otherwise if we create a task, but the gameinstance is nullptr the garbage collector crashes
		UNebulaFlowTaskSubsystem* TaskSubsystem = gameInstance->GetSubsystem<UNebulaFlowTaskSubsystem>();
		if (TaskSubsystem)
		{
			TArray<UNebulaFlowTaskInterface*> TaskList = TaskSubsystem->GetTaskList();
			for (auto TaskInterface : TaskList)
			{
				if (TaskInterface->IsA(UNebulaFlowAppletShownTask::StaticClass()))
				{
					return;
				}
			}
			UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowAppletShownTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::ESwitchAppletShown));
		}
	}
}

#endif

void UNebulaFlowUserSubSystem::HandleDeviceConnected(void* iDevice)
{
	UE_LOG(LogClass, Warning, TEXT("Task: HandleDeviceConnected"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowDeviceConnectedTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::EDeviceConnected), iDevice);
}

void UNebulaFlowUserSubSystem::HandleSaveFailed()
{
	UE_LOG(LogClass, Warning, TEXT("Task: SaveFailed"));
	UNebulaFlowTaskSubsystem::CreateTask<UNebulaFlowSaveFailedTask>(this, GetGameInstance(), UserTaskConfigurationMap.Find(EUserTaskType::ESaveFailed));
}

void UNebulaFlowUserSubSystem::HandleOnConstrained()
{
	UE_LOG(LogClass, Warning, TEXT("Task: HandleOnConstrained"));
	InConstrainedMode = true;
}

void UNebulaFlowUserSubSystem::HandleOnUnconstrained()
{
	UE_LOG(LogClass, Warning, TEXT("Task: HandleOnUnconstrained"));
	InConstrainedMode = false;
}

void UNebulaFlowUserSubSystem::InitializeOnConnectionChangedHandler()
{
	const auto OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
	if (OnlineSub)
	{
		OnlineSub->AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate::CreateUObject(this, &UNebulaFlowUserSubSystem::HandleOnConnectionChange));
	}
}

void UNebulaFlowUserSubSystem::HandleOnConnectionChange(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus)
{
	UE_LOG(LogOnlineGame, Log, TEXT("Connection Status: %s"), EOnlineServerConnectionStatus::ToString(ConnectionStatus));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Connection Status: %s"), EOnlineServerConnectionStatus::ToString(ConnectionStatus)));

	ANebulaFlowPlayerController* playerController = Cast< ANebulaFlowPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (playerController)
	{
		if (ConnectionStatus == EOnlineServerConnectionStatus::Connected)
		{
			playerController->QueryAchievements(true);
		}
	}
}

void UNebulaFlowUserSubSystem::InitializeConfiguration()
{
	UDataTable* Configuration = ConfigurationPath.LoadSynchronous();
	check(Configuration);
	TArray<FNebulaFlowUserTaskConfiguration*> userTaskConfigurations;
	Configuration->GetAllRows(TEXT("NebulaFlowUserSubsystem"), userTaskConfigurations);
	for (auto* userTaskConfiguration : userTaskConfigurations)
	{
		// User Task Configuration Datatable cannot contains the same type multiple times
		check(userTaskConfiguration && !UserTaskConfigurationMap.Contains(userTaskConfiguration->TaskType));
		UserTaskConfigurationMap.Add(userTaskConfiguration->TaskType, *userTaskConfiguration);
	}
}

void UNebulaFlowUserSubSystem::InitializeUserLoginStatusChangedHandler(const IOnlineIdentityPtr& identityInterface)
{
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		identityInterface->AddOnLoginStatusChangedDelegate_Handle(i, FOnLoginStatusChangedDelegate::CreateUObject(this, &ThisClass::HandleUserLoginStatusChanged));
	}
}

void UNebulaFlowUserSubSystem::InitializeControllerPairingChangedHandler(const IOnlineIdentityPtr& identityInterface)
{
	identityInterface->AddOnControllerPairingChangedDelegate_Handle(FOnControllerPairingChangedDelegate::CreateUObject(this, &ThisClass::HandleControllerPairingChanged));
}

void UNebulaFlowUserSubSystem::InitializeControllerConnectionChangedHandler()
{
	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this, &ThisClass::HandleControllerConnectionChanged);
}

void UNebulaFlowUserSubSystem::InitializeOnLoginCompleteHandler(const IOnlineIdentityPtr& identityInterface)
{
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		identityInterface->AddOnLoginCompleteDelegate_Handle(i, FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::HandleOnLoginComplete));
	}
}

void UNebulaFlowUserSubSystem::InitializeOnLoginChangedHandler(const IOnlineIdentityPtr& identityInterface)
{
	identityInterface->AddOnLoginChangedDelegate_Handle(FOnLoginChangedDelegate::CreateUObject(this, &ThisClass::HandleOnLoginChange));
}

void UNebulaFlowUserSubSystem::InitializeOnAppSuspendHandler()
{
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &ThisClass::HandleAppSuspend);
}

void UNebulaFlowUserSubSystem::InitializeOnAppResumeHandler()
{
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &ThisClass::HandleAppResume);
}

void UNebulaFlowUserSubSystem::InitializeOnDeviceConnectedHandler()
{
	//FCoreDelegates::OnDeviceConnected.AddUObject(this, &ThisClass::HandleDeviceConnected);
}

void UNebulaFlowUserSubSystem::InitializeOnSaveFailedHandler()
{
	FNebulaFlowCoreDelegates::OnSaveFailed.AddUObject(this, &ThisClass::HandleSaveFailed);
}

void UNebulaFlowUserSubSystem::InitializeOnConstrained()
{
	FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &ThisClass::HandleOnConstrained);
}

void UNebulaFlowUserSubSystem::InitializeOnUnconstrained()
{
	FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &ThisClass::HandleOnUnconstrained);
}

#if PLATFORM_SWITCH

void UNebulaFlowUserSubSystem::OnConnectedJoyconsUpdate(const int JoyconsConnected)
{
	SetSwitchConnectedControllers(JoyconsConnected);

	if (GetInGamePlayers() > GetSwitchConnectedControllers() && GetSwitchConnectedControllers() > 0)
	{
		if (CreateTask == false)
		{
			CreateTask = true;
		}
	}
	else
	{
		CreateTask = false;
	}
}

void UNebulaFlowUserSubSystem::OnSwitchAppletShown()
{
	bShowingAppletUpdate = true;
}

#endif
