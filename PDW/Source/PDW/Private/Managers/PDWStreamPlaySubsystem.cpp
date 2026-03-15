// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/PDWStreamPlaySubsystem.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
#include "Switch2Application.h"
#endif

#if defined(WITH_NINTENDO_STREAMPLAY) && WITH_NINTENDO_STREAMPLAY
#include "StreamPlay.h"
#endif

UPDWStreamPlaySubsystem* UPDWStreamPlaySubsystem::Get(UObject* WorldContextObject)
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UPDWStreamPlaySubsystem>();
	}

	return nullptr;
}

bool UPDWStreamPlaySubsystem::IsAvailable() const
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2 && defined(WITH_NINTENDO_STREAMPLAY) && WITH_NINTENDO_STREAMPLAY
	return FStreamPlay::Get() != nullptr;
#else
	return false;
#endif
}

bool UPDWStreamPlaySubsystem::IsSessionActive() const
{
	return bIsGuestSessionActive;
}

bool UPDWStreamPlaySubsystem::IsSessionPending() const
{
	return bIsGuestSessionPending;
}

bool UPDWStreamPlaySubsystem::StartGuestSession()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2 && defined(WITH_NINTENDO_STREAMPLAY) && WITH_NINTENDO_STREAMPLAY
	BindPlatformDelegates();

	FStreamPlay* StreamPlay = FStreamPlay::Get();
	if (StreamPlay == nullptr)
	{
		return false;
	}

	StreamPlay->SetOwningAccountUidFromControllerId(0);
	StreamPlay->SetStreamPlayConfiguration(EStreamMode::Mirroring, EStreamType::Local);
	StreamPlay->SetStreamPlayMatchmakeConfiguration(1, 1, 1, 1, false, false, EScreenSharingSettings::None, 0);
	StreamPlay->SetGripTypeAndPadStyle(EGuestJoyHoldType::Horizontal, static_cast<int32>(EGuestPadStyleMask::FullKey));
	StreamPlay->SetHidFeatures(static_cast<int32>(EGuestHidFeatureMask::None));
	StreamPlay->ShowSplayGuestMatchingUI();
	bIsGuestSessionPending = true;
	OnSessionChanged.Broadcast(false);

	return true;
#else
	return false;
#endif
}

void UPDWStreamPlaySubsystem::StopGuestSession()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2 && defined(WITH_NINTENDO_STREAMPLAY) && WITH_NINTENDO_STREAMPLAY
	bIsGuestSessionPending = false;

	if (FStreamPlay* StreamPlay = FStreamPlay::Get())
	{
		StreamPlay->StopSession();
	}

	bIsGuestSessionActive = false;
	OnSessionChanged.Broadcast(false);
#endif
}

void UPDWStreamPlaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	BindPlatformDelegates();
}

void UPDWStreamPlaySubsystem::Deinitialize()
{
	UnbindPlatformDelegates();
	Super::Deinitialize();
}

void UPDWStreamPlaySubsystem::BindPlatformDelegates()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (bHasBoundPlatformDelegates)
	{
		return;
	}

	if (FSwitch2Application* Switch2Application = FSwitch2Application::GetSwitch2Application())
	{
		Switch2Application->OnStreamPlayInitialized.AddUObject(this, &ThisClass::OnStreamPlayInitialized);
		bHasBoundPlatformDelegates = true;
	}
#endif
}

void UPDWStreamPlaySubsystem::UnbindPlatformDelegates()
{
#if defined(PLATFORM_SWITCH2) && PLATFORM_SWITCH2
	if (bHasBoundPlatformDelegates)
	{
		if (FSwitch2Application* Switch2Application = FSwitch2Application::GetSwitch2Application())
		{
			Switch2Application->OnStreamPlayInitialized.RemoveAll(this);
		}
	}
#endif

	bHasBoundPlatformDelegates = false;
}

void UPDWStreamPlaySubsystem::OnStreamPlayInitialized(const bool bStreamPlayInitialized)
{
	bIsGuestSessionPending = false;
	bIsGuestSessionActive = bStreamPlayInitialized;
	OnSessionChanged.Broadcast(bStreamPlayInitialized);
}
