
#pragma once

#include "UI/NebulaFlowDialog.h"
#include "Templates/Function.h"
#include "NebulaFlowManagerBase.h"
#include "TimerManager.h"
#include "NebulaFlowUIManager.generated.h"

class UNebulaFlowBasePage;
class UNebulaFlowLocalPlayer;
class ANebulaFlowPlayerController;
class UNebulaFlowDialog;
class UGameInstance;
class UTexture2D;

UCLASS()
class NEBULAFLOW_API UNebulaFlowUIManager : public UNebulaFlowManagerBase
{

	GENERATED_BODY()

	friend class UNebulaFlowUIFunctionLibrary;

public:

	UNebulaFlowUIManager(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable,Category = "Flow|UI")
	const UNebulaFlowBasePage* GetCurrentDisplayedPage() const {return CurrentDisplayedPageRef;}

//Dialogs

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	bool IsSystemDialogOpened() {return CurrentSystemDialog !=nullptr;}
	
	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	bool IsGameDialogOpened() { return CurrentGameDialog != nullptr;}

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	bool IsAnyDialogOpened() { return (CurrentGameDialog != nullptr || CurrentSystemDialog != nullptr); }

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	bool IsAnyDialogInputLocked() { return (bGameDialogInputLocked || bSystemDialogInputLocked); }

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	bool IsSystemDialogInputLocked() { return (bSystemDialogInputLocked); }

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	UNebulaFlowDialog* GetCurrentGameDialog(){return CurrentGameDialog;}

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	UNebulaFlowDialog* GetCurrentSystemDialog() { return CurrentGameDialog; }

	virtual void InitManager(UNebulaFlowGameInstance* InstanceOwner);

	bool IsUIInputLocked();

	void ClearCachedFocusedWidget();

	bool AllowViewportFocus() const { return bAllowViewportFocus; }
	void AllowViewportFocus(bool val) { bAllowViewportFocus = val; }
protected:

	UPROPERTY()
	UNebulaFlowBasePage* CurrentDisplayedPageRef = nullptr;

	UPROPERTY()
	UNebulaFlowDialog* CurrentGameDialog = nullptr;

	UPROPERTY()
	UNebulaFlowDialog* CurrentSystemDialog = nullptr;

	UPROPERTY()
	UNebulaFlowDialog* CurrentPopup = nullptr;

	bool bGameDialogInputLocked = false ;

	bool bSystemDialogInputLocked = false;

	virtual UNebulaFlowDialog* ShowDialog(UGameInstance* GInstance, APlayerController* Owner, FName DialogID, TFunction<void(FString)> InCallBack,TArray<ANebulaFlowPlayerController*> SyncControllers = {}, TArray<FText> Params = {}, UTexture2D* Image = nullptr);

	UFUNCTION(BlueprintCallable)
	virtual bool HideDialog(UGameInstance* GInstance,UNebulaFlowDialog* DialogToClose);

	UFUNCTION()
	void ReleaseGameDialogInputLock();

	UFUNCTION()
	void ReleaseSystemDialogInputLock();

	bool CloseGameDialog(UGameInstance* GInstance);

	bool CloseSystemDialog(UGameInstance* GInstance);

	void SetAllUsersFocusOnWidget(UUserWidget* WidgetToFocus);

	virtual void OnLoadLevelStarted(FName LevelName);

	virtual void TickManager(float DeltaTime) override ;

private:
	
	void RestoreFocus();

	FTimerManager* UITimerManager;

	// disabled to fix the focus loss on pad disconnection while changing page
	UFUNCTION()
		void OnWidgetFocus(UWidget* FocusedWidget , int32 PlayerIndex);

	UFUNCTION()
 		void OnFirstWidgetFocus(UWidget* FocusedWidget , APlayerController* PlayerController);

	UFUNCTION()
		void OnViewportFocus();

	UPROPERTY()
		TMap<APlayerController*, UWidget*> CachedWidgetsFocus = {};
	UPROPERTY()
		bool bAllowViewportFocus = false;

	UPROPERTY()
		bool bViewportHasFocus = false;

};