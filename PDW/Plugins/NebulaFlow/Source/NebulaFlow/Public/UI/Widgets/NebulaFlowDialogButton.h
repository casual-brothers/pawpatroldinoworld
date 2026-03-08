

#pragma once


#include "UI/NebulaFlowDialog.h"
#include "NebulaFlowNavButtonBase.h"
#include "NebulaFlowDialogButton.generated.h"

class UNebulaFlowNavigationIcon;
class UButton;
class UTextBlock;
class APlayerController;
class UNebulaFlowDialog;
class UNebulaFlowNavbarButton;

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowDialogButton : public UNebulaFlowNavButtonBase
{
	
	GENERATED_BODY()

public:
	
	void InitializeDialogButton(UNebulaFlowDialog* InDialogOwner,APlayerController* InControllerOwner, FDialogButtonConfiguration ButtonConf,EDialogType InDialogType, TArray<ANebulaFlowPlayerController*> SyncControllers = {});

protected:

	void OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	void OnInputStarted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	void OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	void OnInputCompleted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	void OnInputCancelled(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	void OnButtonTriggered() override;

	UPROPERTY()
	UNebulaFlowDialog* DialogOwner;

	FDialogButtonConfiguration CachedButtonConfiguration;
	
	bool bIsSystemDialog = false;
};