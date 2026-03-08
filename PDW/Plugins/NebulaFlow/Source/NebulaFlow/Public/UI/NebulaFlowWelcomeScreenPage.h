// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once



#include "NebulaFlowBasePage.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "NebulaFlowWelcomeScreenPage.generated.h"

class UNebulaFlowGameInstance;

UCLASS()
class NEBULAFLOW_API UNebulaFlowWelcomeScreenPage : public UNebulaFlowBasePage
{
	
	GENERATED_BODY()

protected:

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void AdvanceToMainMenu(int ControllerIndex);

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	FString ProceedAction = "ProceedToMenu";

	UPROPERTY(EditDefaultsOnly)
	bool bAllowAnyKeyToProceed = false;

private:

	UPROPERTY()
	UNebulaFlowGameInstance* GameInstance = nullptr;


};