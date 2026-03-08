

#include "UI/Widgets/NebulaFlowDialogButton.h"
#include "NebulaFlow.h"
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Core/NebulaFlowPlayerInput.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowUIManager.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Core/NebulaFlowEnhancedPlayerInput.h"

void UNebulaFlowDialogButton::InitializeDialogButton(UNebulaFlowDialog* InDialogOwner, APlayerController* InControllerOwner, FDialogButtonConfiguration ButtonConf, EDialogType InDialogType, TArray<ANebulaFlowPlayerController*> SyncControllers)
{
	DialogOwner = InDialogOwner;
	CachedButtonConfiguration = ButtonConf;
	bIsSystemDialog = InDialogType == EDialogType::ESystemDialog;
	InitializeButton(ButtonConf.NavButtonData);
}

void UNebulaFlowDialogButton::OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
	if (UIManager->IsSystemDialogInputLocked())
	{
		if (!bIsSystemDialog)
		{
			return;
		}
	}
	Super::OnInputTriggered(Instance, inPC);
}

void UNebulaFlowDialogButton::OnInputStarted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
	if (UIManager->IsSystemDialogInputLocked())
	{
		if (!bIsSystemDialog)
		{
			return;
		}
	}
	Super::OnInputStarted(Instance, inPC);
}

void UNebulaFlowDialogButton::OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
	if (UIManager->IsSystemDialogInputLocked())
	{
		if (!bIsSystemDialog)
		{
			return;
		}
	}
	Super::OnInputGoing(Instance, inPC);
}

void UNebulaFlowDialogButton::OnInputCompleted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
	if (UIManager->IsSystemDialogInputLocked())
	{
		if (!bIsSystemDialog)
		{
			return;
		}
	}
	Super::OnInputCompleted(Instance, inPC);
}

void UNebulaFlowDialogButton::OnInputCancelled(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
	if (UIManager->IsSystemDialogInputLocked())
	{
		if (!bIsSystemDialog)
		{
			return;
		}
	}
	Super::OnInputCancelled(Instance, inPC);
}

void UNebulaFlowDialogButton::OnButtonTriggered()
{
	Super::OnButtonTriggered();
	
	if (DialogOwner)
	{
		DialogOwner->SendDialogResponse(CachedButtonConfiguration.ButtonResponse, InputTriggerSender,CachedButtonConfiguration.ResponsePriority);
	}
}