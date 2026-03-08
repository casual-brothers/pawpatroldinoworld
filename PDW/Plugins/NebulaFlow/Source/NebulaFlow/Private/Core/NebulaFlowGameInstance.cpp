#include "Core/NebulaFlowGameInstance.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "NebulaFlow.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UI/NebulaFlowUIConstants.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "UObject/UnrealType.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/Class.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "UI/NebulaFlowLoadingPage.h"
#include "MoviePlayer.h"
#include "UI/Widgets/NebulaFlowSaveIcon.h"
#include "Managers/NebulaFlowIntentManager.h"
#include "Online/CoreOnline.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "ErrorsManagement/NebulaFlowProfileChangedErrorInstance.h"
#include "HAL/PlatformInput.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "Online.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"


UNebulaFlowGameInstance::UNebulaFlowGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	
{
}


void UNebulaFlowGameInstance::Init()
{
	Super::Init();
	
	// Used to make Game instance tick
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNebulaFlowGameInstance::Tick));

	//Ensure all class and assets are correctly defined

	ensure(FlowFSMManagerClass);
	ensure(FlowUIManagerClass);
	ensure(FlowErrorManagerClass);
	ensure(UIConstantsDataAsset);
	ensure(FlowAudioManagerClass);

	//InitSingletons

	ErrorManagerInstance = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowErrorManager(this);
	FSMManagerInstance = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(this);
	AudioManagerInstance = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(this);
	AchievementManagerInstance = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAchievementManager(this);

	
	if (UNebulaFlowActivitiesManager::IsActivitiesAvailableOnCurrentPlatform() && IntentManagerClass && !FlowIntentManagerInstance && IsIntentSystemActive)
	{
		FlowIntentManagerInstance = NewObject<UNebulaFlowIntentManager>(this, IntentManagerClass);
		FlowIntentManagerInstance->Init();
	}
	
	//InitDelegates

	FNebulaFlowCoreDelegates::OnLevelLoadStart.AddUObject(this,&UNebulaFlowGameInstance::OnLoadLevelStarted);
	FNebulaFlowCoreDelegates::OnLevelLoaded.AddUObject(this, &UNebulaFlowGameInstance::OnLoadLevelEnded);
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UNebulaFlowGameInstance::ShowLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNebulaFlowGameInstance::HideLoadingScreen);

	UserSubsystem = GetSubsystem<UNebulaFlowUserSubSystem>();

	const auto IdentityInterface = UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity();
	check(IdentityInterface.IsValid());
	//const auto SessionInterface = OnlineSub->GetSessionInterface();
	//check(SessionInterface.IsValid());

	auto onlineIdentityInterface = Online::GetIdentityInterface(TEXT("EOS"));
	if (onlineIdentityInterface)
	{
		StartAutoLoginFlow();
	}

#if PLATFORM_PS4 || (defined(PLATFORM_PS5) && PLATFORM_PS5 == 1)
	if (FPlatformInput::GetGamepadAcceptKey() == EKeys::Gamepad_FaceButton_Right)	// Button Accept & Back are swapped, use secondary txt
	{
		SetIsPS_JP(true);
	}
#endif


#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnRemapJoyconsUIClosed.AddUObject(this, &UNebulaFlowGameInstance::OnRemapJoyConClosed);
#endif
}


void UNebulaFlowGameInstance::StartAutoLoginFlow()
{
	auto onlineIdentityInterface = Online::GetIdentityInterface(TEXT("EOS"));
	if (onlineIdentityInterface)
	{
		EOSAutoLoginInProgress = true;
		int32 idx = 0;//???
		OnAutoLoginCompleteDelegateHandle = onlineIdentityInterface->AddOnLoginCompleteDelegate_Handle(idx, FOnLoginCompleteDelegate::CreateUObject(this, &UNebulaFlowGameInstance::OnAutoLoginComplete));
		onlineIdentityInterface->AutoLogin(idx);
	}
}


void UNebulaFlowGameInstance::OnLoginCompleteShared(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	const auto OnlineSub = IOnlineSubsystem::Get();
	//#EOS todo
	//OnlineSub->DlcInit(); 
}

void UNebulaFlowGameInstance::OnAutoLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	EOSAutoLoginInProgress = false;
	IOnlineIdentityPtr Identity = Online::GetIdentityInterface();
	if (Identity.IsValid())
	{
		Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, OnAutoLoginCompleteDelegateHandle);
	}
	if(bWasSuccessful)
		OnLoginCompleteShared(LocalUserNum, bWasSuccessful, UserId, Error);
}

void UNebulaFlowGameInstance::Shutdown()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	//uninit managers
	
	for (UNebulaFlowManagerBase* currentManager : NebulaFlowManagers)
	{
		if (currentManager)
		{
			currentManager->UninitManager();
		}
	}
	NebulaFlowManagers.Empty();
	Super::Shutdown();
}

void UNebulaFlowGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

void UNebulaFlowGameInstance::RegisterNebulaFlowManager(UNebulaFlowManagerBase* NewManager)
{
	if(!NebulaFlowManagers.Contains(NewManager))
	{ 
		NebulaFlowManagers.AddUnique(NewManager);
		return;
	}
	UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Manager already registered"));
}

void UNebulaFlowGameInstance::ShowSaveIcon()
{
	if (SaveIconWidgetInstance)
	{
		HideSaveIcon();
	}	
	if (!SaveIconWidgetInstance)
	{
		ensure(UIConstantsDataAsset && UIConstantsDataAsset->SaveIconWidgetClass);
		if (UIConstantsDataAsset && UIConstantsDataAsset->SaveIconWidgetClass)
		{
			SaveIconWidgetInstance = CreateWidget<UNebulaFlowSaveIcon>(this, UIConstantsDataAsset->SaveIconWidgetClass);
			UGameViewportClient* pViewport = GetGameViewportClient();
			if (SaveIconWidgetInstance && pViewport)
			{
				SaveIconWidgetInstance->IniatializeSaveIcon(this);
				TSharedPtr<SWidget> SafeWidget;
				if (SaveIconWidgetInstance)
				{
					SafeWidget = SaveIconWidgetInstance->TakeWidget();
					pViewport->AddViewportWidgetContent(SafeWidget.ToSharedRef(), UIConstantsDataAsset->UILayerZOrderMap.Contains(EUILayers::ESaveGameIcon) ? UIConstantsDataAsset->UILayerZOrderMap[EUILayers::ESaveGameIcon] : 11);
				}
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowFSM, Warning, TEXT("No SaveIcon Widget Class Defined!"));
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Already Showing SaveIcon!"));
	}
}


void UNebulaFlowGameInstance::HideSaveIcon()
{
	if (SaveIconWidgetInstance)
	{
		UGameViewportClient* pViewport = GetGameViewportClient();
		if (SaveIconWidgetInstance && pViewport)
		{
			TSharedPtr<SWidget> SafeWidget;
			if (SaveIconWidgetInstance)
			{
				SafeWidget = SaveIconWidgetInstance->TakeWidget();
				pViewport->RemoveViewportWidgetContent(SafeWidget.ToSharedRef());
			}
		}
	}
	SaveIconWidgetInstance = nullptr;
}

int32 UNebulaFlowGameInstance::FindLocalPlayerIndexByControllerId(const int32 ControllerId) const
{
	for (int i = 0; i < LocalPlayers.Num(); ++i)
	{
		if (LocalPlayers[i] && (LocalPlayers[i]->GetControllerId() == ControllerId))
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UNebulaFlowGameInstance::OnPersistentUserLoaded(int32 ControllerId)
{
	OnPersistentUserLoadedDelegate.Broadcast(ControllerId);
}

bool UNebulaFlowGameInstance::IsEnhancedInputActive()
{
	bool bResult = false;
	UNebulaFlowLocalPlayer* NebulaLocalPlayer = GetSubsystem<UNebulaFlowUserSubSystem>()->GetPlayerOwner();
	if (NebulaLocalPlayer)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NebulaLocalPlayer);
		if (Subsystem)
		{
			UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
			if (PlayerInput)
			{
				bResult = true;
			}
		}
	}
	
	return bResult;
}

void UNebulaFlowGameInstance::SetGamePaused(bool bIsPaused)
{
	IsGamePaused = bIsPaused;
	if (IsGamePaused)
	{
		UGameplayStatics::SetGamePaused(this, true);		
	}
	else
	{
		if (!IsSystemPaused)
		{
			UGameplayStatics::SetGamePaused(this, false);						
		}
	}
}

void UNebulaFlowGameInstance::SetSystemPaused(bool bIsPaused)
{
	IsSystemPaused = bIsPaused;
	if (IsSystemPaused)
	{
		UGameplayStatics::SetGamePaused(this, true);
	}
	else
	{
		if (!IsGamePaused)
		{
			UGameplayStatics::SetGamePaused(this, false);
		}
	}
}

bool UNebulaFlowGameInstance::Tick(float DeltaSeconds)
{
	if (UserSubsystem)
	{
		UserSubsystem->Tick(DeltaSeconds);
	}

	if (bIsLoadingLevel)
	{
		return true;
	}

#if PLATFORM_SWITCH
	if (bJustClosedRemapJoyCon)
	{
		bJustClosedRemapJoyCon = false;
		OnSwitchRemapJoyConClosed.Broadcast(UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0));
	}
#endif

	if (SaveIconWidgetInstance)
	{
		SaveIconWidgetInstance->UpdateSaveIcon(DeltaSeconds);
	}
	//Make managers tick:	
	for (UNebulaFlowManagerBase* currentManager : NebulaFlowManagers)
	{
		if(currentManager && currentManager->ShouldManagerTick())
		{ 
			currentManager->TickManager(DeltaSeconds);
		}
	}

	return true; 
}

void UNebulaFlowGameInstance::ShowLoadingScreen(const FString& MapName)
{
	ensure(!LoadingScreenWidgetInstance);
	if(!LoadingScreenWidgetInstance && PendingLevelName!=NAME_None)
	{ 				
		ensure(UIConstantsDataAsset && UIConstantsDataAsset->DefaultLoadingPageClass);
		if (UIConstantsDataAsset && UIConstantsDataAsset->DefaultLoadingPageClass)
		{
			TSubclassOf<UNebulaFlowLoadingPage> PageClass= GetLoadingClass();
			if (UIConstantsDataAsset->MapToLoadingPage.Contains(PendingLevelName))
			{
				PageClass = UIConstantsDataAsset->MapToLoadingPage[PendingLevelName];
			}
			LoadingScreenWidgetInstance = CreateWidget<UNebulaFlowLoadingPage>(this, PageClass);
			UGameViewportClient* pViewport = GetGameViewportClient();
			if (LoadingScreenWidgetInstance && pViewport)
			{
				LoadingScreenWidgetInstance->InitializeLoadingPage(PendingLevelName);
				TSharedPtr<SWidget> SafeWidget;
				if (LoadingScreenWidgetInstance)
				{
					SafeWidget = LoadingScreenWidgetInstance->TakeWidget();
					if(UIConstantsDataAsset->bUseAsyncLoadingPage)
					{
						SetupAsyncLoadingPage();
					}
					else
					{ 
						pViewport->AddViewportWidgetContent(SafeWidget.ToSharedRef(), UIConstantsDataAsset->UILayerZOrderMap.Contains(EUILayers::ELoading) ? UIConstantsDataAsset->UILayerZOrderMap[EUILayers::ELoading] : 10);
					}				
				}
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowFSM, Warning, TEXT("No loading Screen Widget Class Defined!"));
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Already Showing a Loading Screen!"));
	}
}

void UNebulaFlowGameInstance::HideLoadingScreen(UWorld* InLoadedWorld)
{
	if (LoadingScreenWidgetInstance)
	{
		UGameViewportClient* pViewport = GetGameViewportClient();
		if (LoadingScreenWidgetInstance && pViewport)
		{
			TSharedPtr<SWidget> SafeWidget;
			if (LoadingScreenWidgetInstance)
			{
				SafeWidget = LoadingScreenWidgetInstance->TakeWidget();
				pViewport->RemoveViewportWidgetContent(SafeWidget.ToSharedRef());
			}
		}
	}
	LoadingScreenWidgetInstance = nullptr;
}

void UNebulaFlowGameInstance::OnLoadLevelStarted(FName LevelName)
{
	bIsLoadingLevel = true;
	PendingLevelName = LevelName;
	ChangeGameContext(EGameContext::ELoading);
}




void UNebulaFlowGameInstance::OnLoadLevelEnded(FName LevelName)
{
	bIsLoadingLevel = false;
	if (MapToGameContexts.Contains(LevelName))
	{
		ChangeGameContext(MapToGameContexts[LevelName]);
	}
	else
	{
		ChangeGameContext(EGameContext::EGameplay);
	}
}

void UNebulaFlowGameInstance::ChangeGameContext(EGameContext NewContext)
{
	bool isChangingContext = false;
	if (NewContext != CurrentGameContext)
	{
		isChangingContext = true;
	}
	CurrentGameContext = NewContext;
	if(isChangingContext)
	{ 
		OnGameContextChanged.Broadcast(NewContext);
	}
}

void UNebulaFlowGameInstance::SetupAsyncLoadingPage()
{
	if (LoadingScreenWidgetInstance)
	{
		TSharedPtr<SWidget> SafeWidget;
		SafeWidget = LoadingScreenWidgetInstance->TakeWidget();

		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAllowEngineTick = true;
#if PLATFORM_PS4 ||PLATFORM_SWITCH ||PLATFORM_XBOXONE||PLATFORM_PS5||PLATFORM_XSX
		LoadingScreen.bAllowEngineTick = false;
#endif

		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.bMoviesAreSkippable = false;
		LoadingScreen.WidgetLoadingScreen = SafeWidget;
		if (GetMoviePlayer())
		{
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		}
	}
}

void UNebulaFlowGameInstance::HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId)
{
	if (CurrentGameContext == EGameContext::EIntro || CurrentGameContext == EGameContext::EWelcomeScreen)
	{
		return;
	}

	const bool bDowngraded = (LoginStatus == ELoginStatus::NotLoggedIn);

	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bIsLicensed = GenericApplication->ApplicationLicenseValid();

	// Find the local player associated with this unique net id
	ULocalPlayer* LocalPlayer = FindLocalPlayerFromUniqueNetId(UserId);

	if (LocalPlayer != nullptr)
	{
		if (bDowngraded)
		{
			if (LocalPlayer == UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this))
			{
				if (ErrorManagerInstance)
				{
					ErrorManagerInstance->CreateErrorInstance(UNebulaFlowProfileChangedErrorInstance::StaticClass(), UNebulaFlowProfileChangedErrorInstance::PROFILE_CHANGED_DIALOG_ID);
				}
			}
		}
	}
}

TSubclassOf<UNebulaFlowLoadingPage> UNebulaFlowGameInstance::GetLoadingClass()
{
	return  UIConstantsDataAsset->DefaultLoadingPageClass;
}

void UNebulaFlowGameInstance::OnRemapJoyConClosed(bool bSuccess)
{
	bJustClosedRemapJoyCon = true;

	UNebulaFlowConsoleFunctionLibrary::SetIsJoyconHorizontal();
}