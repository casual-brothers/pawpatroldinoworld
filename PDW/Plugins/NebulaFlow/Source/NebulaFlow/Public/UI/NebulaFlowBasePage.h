// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "NebulaFlowBasePage.generated.h"

class UNebulaFlowBaseFSMState;
class APlayerController;
class UNebulaFlowBaseNavbar;
class ANebulaFlowPlayerController;
class UNebulaFlowLocalPlayer;
class UNebulaFlowUIManager;
class UWidgetTree;
class UNebulaFlowPageTemplate;
class UNebulaFlowBaseTopbar;
class UEnhancedInputLocalPlayerSubsystem;
class UNebulaFlowUIConstants;

UENUM()
enum class EAddToScreenType : uint8
{
	EAddToViewPort,
	EAddToPlayerScreen
};

/** it can be stored in a generic enum types class */
UENUM(BlueprintType)
enum class ENavElementPosition : uint8
{
	LEFT,
	CENTER,
	RIGHT
};

UCLASS()
class NEBULAFLOW_API UNebulaFlowBasePage : public UUserWidget
{

	GENERATED_BODY()

		friend class UNebulaFlowUIManager;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
		UNebulaFlowPageTemplate* PageTemplate {};


	UNebulaFlowBasePage(const FObjectInitializer& ObjectInitializer);

	void InitPage(ANebulaFlowPlayerController* InControllerOwner, UNebulaFlowBaseFSMState* InStateOwner);

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
		void TriggerAction(FString Action, FString Parameter, APlayerController* ControllerSender = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
		UNebulaFlowBaseNavbar* GetPageNavbar() { return PageNavbar; }

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
		UNebulaFlowBaseTopbar* GetPageTopbar() { return PageTopbar; }

	void CacheFocusedWidgets();

	void RestoreFocusedWidgets();

	UFUNCTION()
		void DefineTopbarLabel(FText InLabel, ENavElementPosition InPosition);

	UFUNCTION()
		void DefineNavbarButtons(TMap<FName, ENavElementPosition> NavbarConfig);

	UFUNCTION()
		virtual void DefineNavbarLabel(FText InLabel);
	

	// SWITCH
	/** should the remap joycon be active */
	bool IsSwitchRemapJoyconsActive() const;

	/** should the profile swap be active */
	bool IsProfileSwapActive() const;

	/** called when we want to swap the logged in user */
	void ProfileUISwap(const int ControllerIndex) const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
		UNebulaFlowBaseNavbar* PageNavbar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
		UNebulaFlowBaseTopbar* PageTopbar;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;
	virtual void NativePreConstruct()override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY()
		UNebulaFlowBaseFSMState* StateOwnerRef;

	///// SWITCH /////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlatforSpecific|SwitchPlatformSpecific")
		uint32 bAllowSwitchRemapJoycons : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		uint32 bForceAllowSwitchRemapJoycons : 1;

	////// XBOX ///////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlatforSpecific|XBoxPlatformSpecific")
		uint32 bAllowXBoxProfileSwap : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		uint32 bForceAllowXBoxProfileSwap : 1;

	/** delegate for if the profile is swapped */
	void HandleProfileUISwapClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& /*Error*/) const;

	//////// Auto include action mapping ////////
	UPROPERTY(EditAnywhere)
	bool bRemoveActionMappingOnDestruct = true;

	UNebulaFlowUIConstants* UIConstants;;

	UEnhancedInputLocalPlayerSubsystem* EISubsystem{};

private:



	UPROPERTY()
		ANebulaFlowPlayerController* ControllerOwnerRef;

	UPROPERTY()
		UNebulaFlowLocalPlayer* PlayerOwnerRef;

	UPROPERTY()
		TMap<ULocalPlayer*, UWidget*> CachedFocusedWidgets;

	UWidget* FindFocusedWidget(UWidgetTree* InWidgetTree, TSharedRef<SWidget> FindingWidget);

};
