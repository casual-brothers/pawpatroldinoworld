#include "UI/NebulaFlowBasePage.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateUser.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "Components/Widget.h"
#include "HAL/ThreadHeartBeat.h"
#include "FunctionLibraries/NebulaFlowButtonsNameLibrary.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
// just to take the enum. we can move in a generic h
#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "UI/Widgets/NebulaFlowBaseTopbar.h"
#include "UI/NebulaFlowUIConstants.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"

//#include "WidgetBlueprintLibrary.h"

#if (PLATFORM_XBOXONE || PLATFORM_XSX)
#define PROFILE_SWAPPING	1
#else
#define PROFILE_SWAPPING	0
#endif

UNebulaFlowBasePage::UNebulaFlowBasePage(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bAllowXBoxProfileSwap = false;
	bAllowSwitchRemapJoycons = false;

	bForceAllowSwitchRemapJoycons = false;
	bForceAllowXBoxProfileSwap = false;
}

void UNebulaFlowBasePage::InitPage(ANebulaFlowPlayerController* InControllerOwner, UNebulaFlowBaseFSMState* InStateOwner)
{
	StateOwnerRef = InStateOwner;
	ControllerOwnerRef = InControllerOwner;

	UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this);			
	if (!UIConstants && UIConstants->UIInputMapping)
		return;

	UGameInstance* GInstance = UGameplayStatics::GetGameInstance(this);
	for (auto iter = GInstance->GetLocalPlayerIterator(); iter; ++iter)
	{
		ULocalPlayer* player = Cast<ULocalPlayer>(*iter);
		if (player)
		{
			UNebulaFlowLocalPlayer* NebulaLocalPlayer = Cast<UNebulaFlowLocalPlayer>(player);
			if (!ensureMsgf(NebulaLocalPlayer, TEXT("Not a valid NebulaFlowLocalPlayer")))
				continue;

			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NebulaLocalPlayer);
			if (!ensureMsgf(Subsystem, TEXT("Not a valid UEnhancedInputLocalPlayerSubsystem for player")))
				continue;

			if (!Subsystem->HasMappingContext(UIConstants->UIInputMapping))
			{
				Subsystem->AddMappingContext(UIConstants->UIInputMapping, 100);
			}
		}					
	}
	
}

void UNebulaFlowBasePage::TriggerAction(FString Action, FString Parameter, APlayerController* ControllerSender)
{
	if (StateOwnerRef)
	{
		StateOwnerRef->OnFSMStateAction(Action, Parameter, ControllerSender != nullptr ? ControllerSender : ControllerOwnerRef);
		return;
	}
	// LOG NO VALID STATE OWNER FOUND
}

void UNebulaFlowBasePage::CacheFocusedWidgets()
{

	UGameInstance* GInstance = UGameplayStatics::GetGameInstance(this);
	TArray<ULocalPlayer*> LocalPlayers = GInstance->GetLocalPlayers();

	for (ULocalPlayer* current : LocalPlayers)
	{
		TSharedPtr<FSlateUser> SlateUser = current->GetSlateUser();
		if (SlateUser && SlateUser->GetFocusedWidget().IsValid())
		{
			UWidget* UserFocusedWidget = FindFocusedWidget(WidgetTree, SlateUser->GetFocusedWidget().ToSharedRef());
			if (UserFocusedWidget)
			{
				if (!CachedFocusedWidgets.Contains(current))
				{
					CachedFocusedWidgets.Add(current, UserFocusedWidget);
					return;
				}
				CachedFocusedWidgets[current] = UserFocusedWidget;
			}
		}
	}
}

void UNebulaFlowBasePage::RestoreFocusedWidgets()
{
	for (TPair<ULocalPlayer*, UWidget*> current : CachedFocusedWidgets)
	{
		if (current.Key && current.Key->GetPlayerController(GetWorld()) && current.Value)
		{
			current.Value->SetUserFocus(current.Key->GetPlayerController(GetWorld()));
		}
	}
	CachedFocusedWidgets.Empty();
}

void UNebulaFlowBasePage::DefineTopbarLabel(FText InLabel, ENavElementPosition InPosition)
{
	if (PageTopbar== nullptr)
		return;
		
	PageTopbar->SetTitle(InLabel);
	PageTopbar->SetTitlePosition(InPosition);
}

void UNebulaFlowBasePage::DefineNavbarButtons(TMap<FName, ENavElementPosition> InConfig)
{
	if (IsSwitchRemapJoyconsActive())
	{
		InConfig.Add(UNebulaFlowButtonsNameLibrary::SWITCH_REMAP_JOYCONS, ENavElementPosition::CENTER);
	}
	else if (IsProfileSwapActive())
	{
		PageNavbar->SetupSwapProfileLayout();
	}

	PageNavbar->DefineNavbarButtons(InConfig);
}

void UNebulaFlowBasePage::DefineNavbarLabel(FText InLabel)
{
	PageNavbar->DefineNavbarLabel(InLabel);
}


void UNebulaFlowBasePage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CachedFocusedWidgets.Num() == 0)
	{
		CacheFocusedWidgets();
	}
}

void UNebulaFlowBasePage::NativePreConstruct()
{
	Super::NativePreConstruct();
}
//
void UNebulaFlowBasePage::NativeConstruct()
{
	Super::NativeConstruct();
	CachedFocusedWidgets.Empty();

	const UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	ensure(GameInstance);	
	GameInstance->OnPageLoaded.Broadcast();
}

void UNebulaFlowBasePage::NativeDestruct()
{
	if(bRemoveActionMappingOnDestruct)
	{
		UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this);	
		if (UIConstants && UIConstants->UIInputMapping)
		{
			UGameInstance* GInstance = UGameplayStatics::GetGameInstance(this);
			for (auto iter = GInstance->GetLocalPlayerIterator(); iter; ++iter)
			{
				ULocalPlayer* player = Cast<ULocalPlayer>(*iter);
				if (player)
				{
					UNebulaFlowLocalPlayer* NebulaLocalPlayer = Cast<UNebulaFlowLocalPlayer>(player);
					if (!ensureMsgf(NebulaLocalPlayer, TEXT("Not a valid NebulaFlowLocalPlayer")))
						continue;

					UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NebulaLocalPlayer);
					if (!ensureMsgf(Subsystem, TEXT("Not a valid UEnhancedInputLocalPlayerSubsystem for player")))
						continue;

					Subsystem->RemoveMappingContext(UIConstants->UIInputMapping);
				}
			}
		}
	}
	StateOwnerRef = nullptr;
	//in case this was changed for example by the ShowRemapJoycons function
	FThreadHeartBeat::Get().SetDurationMultiplier(1);

	Super::NativeDestruct();
}

bool UNebulaFlowBasePage::IsSwitchRemapJoyconsActive() const
{
#if PLATFORM_SWITCH
	return bAllowSwitchRemapJoycons;
#else
	return bForceAllowSwitchRemapJoycons;
#endif

}

bool UNebulaFlowBasePage::IsProfileSwapActive() const
{
#if PROFILE_SWAPPING
	return bAllowXBoxProfileSwap;
#else
	return bForceAllowXBoxProfileSwap;
#endif
}

void UNebulaFlowBasePage::ProfileUISwap(const int ControllerIndex) const
{
	if (IsProfileSwapActive())
	{
		const FOnLoginUIClosedDelegate Delegate = FOnLoginUIClosedDelegate::CreateUObject(this, &ThisClass::HandleProfileUISwapClosed);
		UNebulaFlowUserSubSystem* UserSystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UNebulaFlowUserSubSystem>();
		if (UserSystem)
		{
			UserSystem->RequireChangeUser(ControllerIndex);
		}
	}
}

void UNebulaFlowBasePage::HandleProfileUISwapClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnlineError& /*Error*/) const
{

}

UWidget* UNebulaFlowBasePage::FindFocusedWidget(UWidgetTree* InWidgetTree, TSharedRef<SWidget> FindingWidget)
{
	UWidget* ResultWidget = InWidgetTree->FindWidget(FindingWidget);
	if (ResultWidget)
	{
		return ResultWidget;
	}

	TArray<UWidget*> Widgets;
	InWidgetTree->GetAllWidgets(Widgets);

	for (auto& Widget : Widgets)
	{
		UUserWidget* UserWidget = Cast<UUserWidget>(Widget);
		if (UserWidget)
		{
			ResultWidget = FindFocusedWidget(UserWidget->WidgetTree, FindingWidget);

			if (ResultWidget)
			{
				return ResultWidget;
			}
		}
	}

	return nullptr;
}

