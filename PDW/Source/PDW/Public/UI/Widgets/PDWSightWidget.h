// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "StructUtils/InstancedStruct.h"
#include "Managers/PDWHUDSubsystem.h"
#include "PDWSightWidget.generated.h"

class UPDWMinigameNavButton;
class UPDWPlayerIndicator;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWSightWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWMinigameNavButton* MinigameNavButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWPlayerIndicator* PlayerIndicator;

	UPROPERTY(BlueprintReadOnly)
	bool bIsTargetFound = false;

	FNavButtonTriggerCallback TriggerCallback = FNavButtonTriggerCallback();
	FNavButtonProgressCallback ProgressCallback = FNavButtonProgressCallback();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "FoundTarget"))
	void BP_FoundTarget(bool Found);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StartMinigameInputActionIcon"))
	void BP_StartMinigameInputActionIcon();
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "RestoreSight"))
	void BP_RestoreSight();

	UFUNCTION()
	void OnNavIconTrigger(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance);

	UFUNCTION()
	void OnNavIconProgress(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance, float PreviousTime, float CurrentTime);

public:

	UFUNCTION(BlueprintCallable)
	void FoundTarget(bool Found);
	
	void StartMinigameInputActionIcon(FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack = FNavButtonTriggerCallback(), FNavButtonProgressCallback InProgressCallBack = FNavButtonProgressCallback());
	
	UFUNCTION(BlueprintCallable)
	void RestoreSight();

	UFUNCTION(BlueprintCallable)
	void InitSight(APDWPlayerController* OwnerWidget);
};