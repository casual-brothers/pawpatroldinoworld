#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowUIManager.h"
#include "Core/NebulaFlowPlayerController.h"

void UNebulaFlowNavbarButton::NativeConstruct()
{
	Super::NativeConstruct();
	UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
}

void UNebulaFlowNavbarButton::OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (UIManager && !UIManager->IsAnyDialogInputLocked())
	{
		Super::OnInputTriggered(Instance, inPC);
	}
}

void UNebulaFlowNavbarButton::OnInputStarted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (UIManager && !UIManager->IsAnyDialogInputLocked())
	{
		Super::OnInputStarted(Instance, inPC);
	}
}

void UNebulaFlowNavbarButton::OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (UIManager && !UIManager->IsAnyDialogInputLocked())
	{
		Super::OnInputGoing(Instance, inPC);
	}
}

void UNebulaFlowNavbarButton::OnInputCompleted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (UIManager && !UIManager->IsAnyDialogInputLocked())
	{
		Super::OnInputCompleted(Instance, inPC);
	}
}

void UNebulaFlowNavbarButton::OnInputCancelled(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (UIManager && !UIManager->IsAnyDialogInputLocked())
	{
		Super::OnInputCancelled(Instance, inPC);
	}
}

void UNebulaFlowNavbarButton::OnClick()
{
	if (UIManager && !UIManager->IsAnyDialogInputLocked())
	{
		Super::OnClick();
	}
}

void UNebulaFlowNavbarButton::OnButtonTriggered()
{
	Super::OnButtonTriggered();
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, NavbarButtonData.ButtonAction.ToString(), FString(""), InputTriggerSender);
}