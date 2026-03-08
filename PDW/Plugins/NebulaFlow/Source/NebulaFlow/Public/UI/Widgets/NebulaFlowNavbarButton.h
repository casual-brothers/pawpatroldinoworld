// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NebulaFlowNavButtonBase.h"
#include "NebulaFlowNavbarButton.generated.h"

class UNebulaFlowUIManager;

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowNavbarButton : public UNebulaFlowNavButtonBase
{
	
	GENERATED_BODY()

	friend class UNebulaFlowBaseNavbar;

public:
	
	UFUNCTION(BlueprintCallable, Category = "FLOW|UI")
	void SetButtonName(FName NewName) {ButtonName = NewName;}

	UFUNCTION(BlueprintCallable, Category = "FLOW|UI")
	const FName GetButtonName() const {return ButtonName;}

protected:

	UPROPERTY()
	UNebulaFlowUIManager* UIManager;

	UPROPERTY()
	FName ButtonName;

	void NativeConstruct() override;
	virtual void OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	virtual void OnInputStarted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	virtual void OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	virtual void OnInputCompleted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	virtual void OnInputCancelled(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;
	virtual void OnClick() override;
	virtual void OnButtonTriggered() override;
};