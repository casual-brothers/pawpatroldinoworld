// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "PDWWaitForSpecifcPup.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Wait for Specific Pup"))
class PDW_API UPDWWaitForSpecifcPup : public UFlowNode
{
	GENERATED_BODY()
	
protected:
	static const FName SUCCES;
	static const FName CLOSEMENUWRONGPUP;
	static const FName CLOSEMENUWRONGONVEHICLE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories="ID.Character"))
	FGameplayTag PupTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOnVehicle = false;
	
	UFUNCTION()
	void CheckPupConfig();

	UFUNCTION()
	void OnChangeCharacter(const FGameplayTag& NewPup, APDWPlayerController* inController);

	UFUNCTION()
	void OnSwapOnVehicle(const bool IsOnVehicle, APDWPlayerController*inController );

	void TriggerOutput(FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;
	
	UFUNCTION()
	void OnSwapCharacterMenuClosed();

	void OnLoad_Implementation() override;

	UFUNCTION()
	void ExecuteNode();

public:
	UPDWWaitForSpecifcPup(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void ExecuteInput(const FName& PinName) override;
};
