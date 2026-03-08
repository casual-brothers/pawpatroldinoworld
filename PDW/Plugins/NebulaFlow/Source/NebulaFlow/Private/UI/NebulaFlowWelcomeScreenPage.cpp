#include "UI/NebulaFlowWelcomeScreenPage.h"
#include "Input/Reply.h"
#include "OnlineSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#if (PLATFORM_XBOXONE || PLATFORM_XSX)
#include "XboxCommonPlatformApplicationMisc.h"
#include "XboxCommonApplication.h"
#include "XboxCommonPlatformInputDeviceMapper.h"
#endif

FReply UNebulaFlowWelcomeScreenPage::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	
#if !((PLATFORM_XBOXONE || PLATFORM_XSX || PLATFORM_WINGDK))		

	if (InKeyEvent.GetUserIndex() != 0)
	{
		return FReply::Unhandled();
	}
#endif

	const FKey Key = InKeyEvent.GetKey();

		

#if (PLATFORM_XBOXONE || PLATFORM_XSX)
	if (InKeyEvent.GetUserIndex() != 0)
	{
		FPlatformUserId PlatformId = FPlatformMisc::GetPlatformUserForUserIndex(0);

		FShowInputDeviceSelectorComplete OnShowInputDeviceSelectorComplete = ([this](const FShowInputDeviceSelectorParams&) {});

		if (!FXboxCommonPlatformApplicationMisc::ShowInputDeviceSelector(PlatformId, (OnShowInputDeviceSelectorComplete)))
		{
			UE_LOG(LogNebulaFlowUI, Warning, TEXT("Failed To Open Input Device Selector UI"));
		}
		return FReply::Unhandled();
	}

#endif

	if (Key == EKeys::Virtual_Accept || Key == EKeys::Enter || bAllowAnyKeyToProceed)
	{
		auto* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
		check(UserSubSystem);
		UserSubSystem->RequireUserLogin(InKeyEvent.GetUserIndex());
	}
	return FReply::Unhandled();

}

void UNebulaFlowWelcomeScreenPage::NativeConstruct()
{
	Super::NativeConstruct();
	GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	auto* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	check(UserSubSystem);
	UserSubSystem->OnUserLoggedInDelegate.AddDynamic(this, &ThisClass::AdvanceToMainMenu);
	SetIsFocusable(true);
	FSlateApplication::Get().SetAllUserFocus(TakeWidget());
}

void UNebulaFlowWelcomeScreenPage::NativeDestruct()
{
	auto* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	check(UserSubSystem);
	UserSubSystem->OnUserLoggedInDelegate.RemoveDynamic(this, &ThisClass::AdvanceToMainMenu);
	Super::NativeDestruct();
}

void UNebulaFlowWelcomeScreenPage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// User Focus Must always be on this page
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!this->HasAnyUserFocus())
	{
		FCoreUObjectDelegates::OnWidgetFocusReceived.Broadcast(this, 0);
		FSlateApplication::Get().SetAllUserFocus(this->TakeWidget());
	}
}

void UNebulaFlowWelcomeScreenPage::AdvanceToMainMenu(int ControllerIndex)
{
	TriggerAction(ProceedAction, FString(""));
}

