// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Blueprint/UserWidget.h"
#include "UI/NebulaFlowWidgetDataStructures.h"
#include "NebulaFlowBaseWidget.generated.h"



UCLASS()
class NEBULAFLOW_API UNebulaFlowBaseWidget : public UUserWidget
{
	
	GENERATED_BODY()

public:
	
	UNebulaFlowBaseWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Flow|UI")
	void TriggerAction(FString Action, FString Parameter, APlayerController* ControllerSender = nullptr);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Platform Visibility")
	FPlatformVisibility PlatformVisibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FUIAudioEvents UIAudioEvents;
		
	// Function to skip the first focus sound on button
	void SetPlayNextFocusAudioEvents(bool bInPlayAudioEvents);

protected:

	virtual void NativePreConstruct() override;

	// Bool to skip the first focus sound on button
	bool bPlayNextFocusAudioEvents = true;

};