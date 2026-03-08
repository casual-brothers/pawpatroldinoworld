// Copyright Epic Games, Inc. All Rights Reserved.
#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/NebulaFlowBasePage.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Core/NebulaFlowPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "UI/NebulaFlowBasePage.h"
#include "HAL/ThreadHeartBeat.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Engine/GameInstance.h"
#include "Containers/Array.h"



UNebulaFlowBaseUIFSMState::UNebulaFlowBaseUIFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNebulaFlowBaseUIFSMState::OnFSMStateEnter_Implementation(const FString& InOption)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	GInstanceRef = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	auto* PlayerOwner = GInstanceRef->GetSubsystem<UNebulaFlowUserSubSystem>()->GetPlayerOwner();
	if (GInstanceRef && PlayerOwner && bShowMouseCursorOnEnter)
	{
		PlayerOwner->GetPlayerController(GInstanceRef->GetWorld())->SetShowMouseCursor(true);
	}
	if (PlayerOwner && bHideMouseCursorOnEnter)
	{
		PlayerOwner->GetPlayerController(GInstanceRef->GetWorld())->SetShowMouseCursor(false);
	}

	DefineStateOwner();

	CreatePage();

	FNebulaFlowCoreDelegates::OnNewPageShowed.Broadcast();
}

void UNebulaFlowBaseUIFSMState::OnFSMStateExit_Implementation()
{
	auto* PlayerOwner = GInstanceRef->GetSubsystem<UNebulaFlowUserSubSystem>()->GetPlayerOwner();
	if (PlayerOwner && bHideMouseCursorOnExit)
	{
		PlayerOwner->GetPlayerController(GInstanceRef->GetWorld())->SetShowMouseCursor(false);
	}
	if (PageRef)
	{
		UNebulaFlowUIFunctionLibrary::RemovePage(this, PageRef);
	}
	Super::OnFSMStateExit_Implementation();
}

void UNebulaFlowBaseUIFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	if (Action == "RemapJoycons")
	{
		ShowRemapJoycons();
	}
	else if (Action == "ProfileSwap")
	{
		if (PageRef->IsProfileSwapActive())
		{
			ULocalPlayer* LocalPlayer = ControllerSender->GetLocalPlayer();
			if (LocalPlayer)
			{
				int32 Index = LocalPlayer->GetControllerId();
				PageRef->ProfileUISwap(Index);
			}
		}
	}
}

void UNebulaFlowBaseUIFSMState::DefineStateOwner()
{
	StateOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetPlayerController(GetWorld());
}

void UNebulaFlowBaseUIFSMState::CreatePage()
{
	ANebulaFlowPlayerController* NebulaOwner = Cast<ANebulaFlowPlayerController>(StateOwner);

	if (PageClass && NebulaOwner)
	{
		PageRef = UNebulaFlowUIFunctionLibrary::CreateNewPage(this, PageClass, this, NebulaOwner);
		//To don't brake retro compatibility I added a second way of set the buttons. if we don't care about it, we can remove the above method to define navbar buttons
		if (PageRef && PageRef->GetPageNavbar() && (NavbarConfigWithPosition.Num() > 0 || !NavbarLabel.IsEmpty()))
		{
			PageRef->GetPageNavbar()->SetOwningPlayer(StateOwner);

			if (NavbarConfigWithPosition.Num() > 0)
				PageRef->DefineNavbarButtons(NavbarConfigWithPosition);

			if (!NavbarLabel.IsEmpty())
				PageRef->DefineNavbarLabel(NavbarLabel);
		}
	}
}

bool UNebulaFlowBaseUIFSMState::ShowRemapJoycons() const
{
	if (PageRef)
	{
		if (PageRef->IsSwitchRemapJoyconsActive())
		{
			UNebulaFlowConsoleFunctionLibrary::ShowRemapJoycons();
			return true;
		}
	}
	return false;
}
