#pragma once


class NEBULAFLOW_API FNebulaFlowCoreDelegates
{
	
public:

//Declarations

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FTriggerActionDelegate, const FString&, const FString&, APlayerController* PController);

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FAnyInputPressed, FKey, bool, APlayerController*);
	
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FAnyInputReleased, FKey , bool, APlayerController* );

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelLoadStart, FName);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelLoaded, FName);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameSaved, bool);

	DECLARE_MULTICAST_DELEGATE(FOnSaveFailed);

	DECLARE_MULTICAST_DELEGATE(FOnBeforSave);

	DECLARE_MULTICAST_DELEGATE(FOnPostLoad)

	DECLARE_MULTICAST_DELEGATE(FOnNewPageShowed);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUserLogOut, int32 ControllerId);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnControllerDisconnect, int32 ControllerId);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGamePaused, bool);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSystemPaused, bool);

	DECLARE_MULTICAST_DELEGATE(FOnActionRemapped);

//Delegates

	static FTriggerActionDelegate OnActionTriggered;

	static FAnyInputReleased OnAnyInputReleased;
	
	static FAnyInputPressed OnAnyInputPressed;

	static FOnLevelLoadStart OnLevelLoadStart;

	static FOnLevelLoaded OnLevelLoaded;

	static FOnGameSaved OnGameSaved;

	static FOnSaveFailed OnSaveFailed;
	
	static FOnBeforSave OnBeforSave;

	static FOnPostLoad OnPostLoad;

	static FOnNewPageShowed OnNewPageShowed;

	static FOnUserLogOut OnUserLogOut;

	static FOnControllerDisconnect OnControllerDisconnect;

	static FOnGamePaused OnGamePaused;

	static FOnSystemPaused OnSystemPaused;

	static FOnActionRemapped OnActionRemapped;
};