// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/PDWRemappingWidgetOverlay.h"
#include "Managers/PDWEventSubsytem.h"
#include "UI/Widgets/PDWRemappingActionButton.h"
#include "Components/CanvasPanel.h"
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "Components/TextBlock.h"
#include "PDWGameInstance.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimationState.h"



void UPDWRemappingWidgetOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonClick.AddUniqueDynamic(this, &UPDWRemappingWidgetOverlay::OnRemappingButtonClick);
		EventSubsystem->OnRemappingButtonNewKeyChosen.AddUniqueDynamic(this, &UPDWRemappingWidgetOverlay::OnRemappingButtonNewKeyChosen);
		EventSubsystem->OnRemappingButtonInvalidNewKeyChosen.AddUniqueDynamic(this, &UPDWRemappingWidgetOverlay::OnRemappingButtonInvalidNewKeyChosen);
	}
#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.AddUniqueDynamic(this, &ThisClass::OnClosedSwitchRemap);
	OnClosedSwitchRemap(UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0));
#endif

	RemapCanvas->SetVisibility(ESlateVisibility::Collapsed);
	InvalidKeyText->SetVisibility(ESlateVisibility::Collapsed);
}

void UPDWRemappingWidgetOverlay::NativeDestruct()
{
	Super::NativeDestruct();

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonClick.RemoveAll(this);
		EventSubsystem->OnRemappingButtonNewKeyChosen.RemoveAll(this);
		EventSubsystem->OnRemappingButtonInvalidNewKeyChosen.RemoveAll(this);
	}
}


void UPDWRemappingWidgetOverlay::OnClosedSwitchRemap(bool bSingleJoyCon)
{
#if PLATFORM_SWITCH
	if (bSingleJoyCon)
	{
		RemapCanvas->SetVisibility(ESlateVisibility::Collapsed);
		InvalidKeyText->SetVisibility(ESlateVisibility::Collapsed);
	}
#endif
}

void UPDWRemappingWidgetOverlay::OnRemappingButtonClick(UPDWRemappingActionButton* RemappingButton)
{
	FWidgetAnimationHandle AnimationHandle = PlayAnimation(OpenAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	if (AnimationHandle.IsValid())
	{
		if (FWidgetAnimationState* AnimState = AnimationHandle.GetAnimationState())
		{
			AnimState->GetOnWidgetAnimationFinished().AddUObject(this, &UPDWRemappingWidgetOverlay::OnOpenAnimationFinished);
		}
	}
	else
	{
		UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
		if (EventSubsystem)
		{
			EventSubsystem->OnOverlayOpenAnimationFinished.Broadcast();
		}
	}

	RemapCanvas->SetVisibility(ESlateVisibility::Visible);
	RemapIcon->SetVisibility(ESlateVisibility::Visible);
	TextRemapActionName->SetText(RemappingButton->InputName->GetText());
	RemapIcon->InitNavigationIcon(RemappingButton->ActionName, 0, 0, RemappingButton->OverriddenActionName);
}

void UPDWRemappingWidgetOverlay::OnOpenAnimationFinished(FWidgetAnimationState& AnimationState)
{
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnOverlayOpenAnimationFinished.Broadcast();
	}
}

void UPDWRemappingWidgetOverlay::OnRemappingButtonNewKeyChosen(UPDWRemappingActionButton* RemappingButton, FKey InputKey)
{
	FWidgetAnimationHandle AnimationHandle = PlayAnimation(CloseAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	if (AnimationHandle.IsValid())
	{
		if (FWidgetAnimationState* AnimState = AnimationHandle.GetAnimationState())
		{
			AnimState->GetOnWidgetAnimationFinished().AddUObject(this, &UPDWRemappingWidgetOverlay::OnCloseAnimationFinished);
		}
	}
	else
	{
		UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
		if (EventSubsystem)
		{
			EventSubsystem->OnOverlayCloseAnimationFinished.Broadcast();
		}
	}
	//RemapCanvas->SetVisibility(ESlateVisibility::Collapsed);
	InvalidKeyText->SetVisibility(ESlateVisibility::Collapsed);
	RemapIcon->SetVisibility(ESlateVisibility::Visible);
	RemapIcon->InitNavigationIcon(RemappingButton->ActionName, 0, 0, RemappingButton->OverriddenActionName);
}

void UPDWRemappingWidgetOverlay::OnCloseAnimationFinished(FWidgetAnimationState& AnimationState)
{
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnOverlayCloseAnimationFinished.Broadcast();
	}
}

void UPDWRemappingWidgetOverlay::OnRemappingButtonInvalidNewKeyChosen(UPDWRemappingActionButton* RemappingButton)
{
	RemapIcon->SetVisibility(ESlateVisibility::Collapsed);
	InvalidKeyText->SetVisibility(ESlateVisibility::Visible);
}

