// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Engine/GameInstance.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "OnlineSubsystemTypes.h"
#include "Online/CoreOnline.h"
#include "Input/NebulaFlowInputDataStructures.h"
#include "NebulaFlowGameInstance.generated.h"


class UNebulaFlowAchievementManager;
class UNebulaFlowActivitiesManager;
class UNebulaFlowAudioManager;
class UNebulaFlowCommandManager;
class UNebulaFlowErrorManager;
class UNebulaFlowFSMManager;
class UNebulaFlowIntentManager;
class UNebulaFlowLoadingPage;
class UNebulaFlowLocalPlayer;
class UNebulaFlowManagerBase;
class UNebulaFlowSaveIcon;
class UNebulaFlowSingletonFunctionLibrary;
class UNebulaFlowUIConstants;
class UNebulaFlowUIManager;
class UUserWidget;
class UNebulaFlowUserSubSystem;

UENUM(BlueprintType)
enum class EGameContext : uint8
{
	EIntro,
	EWelcomeScreen,
	EMainMenu,
	EGameplay,
	ELoading,
	ESaving
};

//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPersistentUserLoadedSignature, int, ControllerId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameContextChanged, EGameContext, NewGameContext);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchRemapJoyConClosed, bool, bSingleJoyCon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPageLoaded);


UCLASS(config = Game)
class NEBULAFLOW_API UNebulaFlowGameInstance : public UGameInstance
{
	friend class UNebulaFlowSingletonFunctionLibrary;
	friend class UNebulaFlowUIFunctionLibrary;
	friend class UNebulaFlowCoreFunctionLibrary;
	friend class UNebulaFlowIntentManager;
	friend class UNebulaFlowActivitiesManager;

	GENERATED_UCLASS_BODY()

public:

	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;

	//Class Definitions

	UPROPERTY(EditDefaultsOnly, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowFSMManager> FlowFSMManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowUIManager> FlowUIManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowErrorManager> FlowErrorManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowAchievementManager> FlowAchievementManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowAudioManager> FlowAudioManagerClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowIntentManager> IntentManagerClass;

	/*Change this to True when Developing PS5 Activities and Intents*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class Definitions|Managers|Intents")
	bool IsIntentSystemActive = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class Definitions|Managers")
	TSubclassOf<UNebulaFlowActivitiesManager> ActivitiesManagerClass;

	void RegisterNebulaFlowManager(UNebulaFlowManagerBase* NewManager);

	UPROPERTY(EditDefaultsOnly, Category ="Maps")
	FName WelcomeScreenMapName = NAME_None;

	//Player Profile Management

	bool IsSaveSystemEnabled(){return bSaveSystemEnabled;}

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ShowSaveIcon();

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void HideSaveIcon();

	UFUNCTION(BlueprintPure, Category = "Player")
	int32 FindLocalPlayerIndexByControllerId(const int32 ControllerId) const;

	virtual void OnPersistentUserLoaded(int32 ControllerId);

	UPROPERTY(BlueprintAssignable)
	FPersistentUserLoadedSignature OnPersistentUserLoadedDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnGameContextChanged OnGameContextChanged;


	//Level Travel

	UFUNCTION(BlueprintCallable)
	EGameContext GetCurrentGameContext(){return CurrentGameContext;}

	//Input Assets

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		UDefaultRemappableActions* RemappableActionsDataAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		UDefaultRemappableActions* RemappableActionsDataAssetDebug = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		TMap<const UInputMappingContext*, const UInputMappingContext*> JapanesePlatformData {};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		UJapaneseInputSetup* JapaneseInputs = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		UMappingContextList* MappingContextDataAsset = nullptr;

	UFUNCTION()
		bool IsEnhancedInputActive();

	bool GetIsPS_JP() const { return bIsPS_JP; }
	void SetIsPS_JP(bool val) { bIsPS_JP = val; }

	void SetGamePaused(bool bIsPaused);
	void SetSystemPaused(bool bPaused);
		
	UPROPERTY(BlueprintReadOnly)
	bool IsUsingGamepad = false;

	void SetCanShowDisconnectControllerDialog(bool bCanShowDialog){ bCanShowDisconnectControllerDialog = bCanShowDialog; }
	bool CanShowDisconnectControllerDialog() const { return bCanShowDisconnectControllerDialog; } 

	//Save Condition types (Cheat)
	UPROPERTY()
		int32 SaveCondition = 1;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Maps|GameContexts")
	TMap<FName, EGameContext> MapToGameContexts;
	
	UPROPERTY(EditDefaultsOnly, Category ="FLOW|UI")
	UNebulaFlowUIConstants* UIConstantsDataAsset;

	//Level Load
	UPROPERTY()
		UNebulaFlowLoadingPage* LoadingScreenWidgetInstance = nullptr;

	UPROPERTY()
		UNebulaFlowSaveIcon* SaveIconWidgetInstance = nullptr;

	virtual bool Tick(float DeltaSeconds);

	FTSTicker::FDelegateHandle TickDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Flow")
	virtual void ShowLoadingScreen(const FString& MapName) ;

	UFUNCTION(BlueprintCallable, Category = "Flow")
	virtual void HideLoadingScreen(UWorld* InLoadedWorld);

	UFUNCTION()
	virtual TSubclassOf<UNebulaFlowLoadingPage> GetLoadingClass();

	//Called Before OpenLevel Call
	UFUNCTION()
	void OnLoadLevelStarted(FName LevelName);
	
	UFUNCTION()
	void OnLoadLevelEnded(FName LevelName);

	UFUNCTION()
	void ChangeGameContext(EGameContext NewContext);

	UFUNCTION()
	virtual void SetupAsyncLoadingPage();

	//Ps5 Activities

	UPROPERTY()
	UNebulaFlowIntentManager* FlowIntentManagerInstance;

	UPROPERTY()
	UNebulaFlowActivitiesManager* FlowActivitiesManagerInstance;

	UPROPERTY()
	UNebulaFlowUserSubSystem* UserSubsystem{};


private:

	//Singletons

	UPROPERTY()
	UNebulaFlowCommandManager* CommandManagerInstance = nullptr;

	UPROPERTY()
	UNebulaFlowFSMManager* FSMManagerInstance = nullptr;

	UPROPERTY()
	UNebulaFlowUIManager* UIManagerInstance = nullptr;

	UPROPERTY()
	UNebulaFlowErrorManager* ErrorManagerInstance = nullptr;

	UPROPERTY()
	UNebulaFlowAchievementManager* AchievementManagerInstance = nullptr;

	UPROPERTY()
	UNebulaFlowAudioManager* AudioManagerInstance = nullptr;

	UPROPERTY()
	TArray<UNebulaFlowManagerBase*> NebulaFlowManagers;

	UPROPERTY()
	EGameContext CurrentGameContext = EGameContext::EIntro;

	//Savings

	bool bSaveSystemEnabled = true;

	UPROPERTY()
	bool bIsLoadingLevel = false;

	UPROPERTY()
	FName PendingLevelName = NAME_None;

	UPROPERTY()
	bool IsSystemPaused = false;

	UPROPERTY()
	bool IsGamePaused = false;

	//USER MANAGEMENT (Maybe to put this logic in a dedicated manager)
	void HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId);

		public:
			bool EOSAutoLoginInProgress = false;
//		bool EOSLoginInProgress = false;
//		const FOnLoginUIClosedDelegate* OnLoginCallback = NULL;

private:
	void StartAutoLoginFlow();

	void OnAutoLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnLoginCompleteShared(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	FDelegateHandle OnAutoLoginCompleteDelegateHandle;
	//	FDelegateHandle OnLoginCompleteDelegateHandle;

	bool bIsLicensed;

	bool bIsPS_JP = false;

	bool bCanShowDisconnectControllerDialog = false;

private:

	UFUNCTION(BlueprintCallable)
	void OnRemapJoyConClosed(bool bSuccess);

	void OnPreLoadMap(const FString& MapName);
	bool bJustClosedRemapJoyCon = false;

public:

	UPROPERTY(BlueprintAssignable)
	FOnSwitchRemapJoyConClosed OnSwitchRemapJoyConClosed;
		
	UPROPERTY(BlueprintAssignable)
	FOnPageLoaded OnPageLoaded;

private:

	bool bIsFreeCameraActive = false;

public:

	bool IsFreeCameraActive() { return bIsFreeCameraActive; }

	void SetFreeCamera(bool bActive) { bIsFreeCameraActive = bActive; }

};