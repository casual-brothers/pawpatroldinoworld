// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FSM/NebulaFlowBaseFSMState.h"
#include "NebulaFlowBaseUIFSMState.generated.h"

class UNebulaFlowBasePage;
class UNebulaFlowGameInstance;
class APlayerController;

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowBaseUIFSMState : public UNebulaFlowBaseFSMState
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString(""));

	virtual void OnFSMStateExit_Implementation();

	virtual void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)override;

	UPROPERTY()
		APlayerController* StateOwner;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UNebulaFlowBasePage> PageClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		bool bShowMouseCursorOnEnter = false;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		bool bHideMouseCursorOnExit = true;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		bool bHideMouseCursorOnEnter = true;

	UPROPERTY(Transient)
		UNebulaFlowBasePage* PageRef = nullptr;

	/** To don't brake retro compatibility I added a second way of set the buttons. if we don't care about it, we can remove the above NavbarConfig */

	UPROPERTY(EditDefaultsOnly, Category = "Flow|UI", meta =(DisplayName="Navbar buttons"))
		TMap<FName, ENavElementPosition> NavbarConfigWithPosition;

	UPROPERTY(EditDefaultsOnly, Category = "Flow|UI")
		FText NavbarLabel;

	virtual void DefineStateOwner();

	virtual void CreatePage();

	UPROPERTY(Transient)
		UNebulaFlowGameInstance* GInstanceRef = nullptr;

	bool ShowRemapJoycons() const;

};
