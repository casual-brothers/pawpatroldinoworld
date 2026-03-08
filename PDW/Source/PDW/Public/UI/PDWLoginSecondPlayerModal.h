// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowDialog.h"
#include "PDWLoginSecondPlayerModal.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWLoginSecondPlayerModal : public UNebulaFlowDialog
{
	GENERATED_BODY()
	
public:
	virtual void InitDialog(APlayerController* InControllerOwner, FName DialogID, TFunction<void(FString)> InCallBack, const TArray<ANebulaFlowPlayerController*>& SyncControllers = {}, const TArray<FText>& Params = {}, UTexture2D* Image = nullptr, bool ibForceFocus = false, TObjectPtr<UInputMappingContext> InputMappingContext = {}) override;

protected:
	bool bCheckConfigurationJoycon = false;

	bool bResultApplet = false;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void ManageCreatePlayer2(int32 UserIndex);

	UFUNCTION()
	void CheckConfigurationJoycon(bool IsRemap);

	void NativeDestruct() override;
};
