// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWRewardPage.generated.h"

class UPDWUnlockWidget;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWRewardPage : public UNebulaFlowBasePage
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* OnLevelUp;

	UPROPERTY(EditAnywhere)
	FName CustomizeNavButtonID = "ChangeCustomization";

	UPROPERTY(EditAnywhere)
	FName EquipNowButtonID = "EquipNow";
	
	UPROPERTY(EditAnywhere)
	FName ConfirmNavButtonID = "Confirm";

	UPROPERTY()
	FPDWUnlockableContainer RewardsToUnlock;

	UPROPERTY()
	int32 CurrentIndex = 0;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWUnlockWidget> UnlockWidget;

	UFUNCTION()
	void ShowCurrentUnlockable();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowCurrentUnlockable(UPDWUnlockableData* Unlockable);

	UFUNCTION()
	void ManageOnEndShowAnimationFinished();

	void NativeDestruct() override;

	void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	
	UFUNCTION(BlueprintCallable)
	bool CheckForNextUnlockable();
	
	UFUNCTION(BlueprintCallable)
	void InitRewardPage(FPDWUnlockableContainer& InRewardsToUnlock, bool bIsFromLevelUp = false);

};
