// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "PDWNavButtonWithParameter.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWNavButtonWithParameter : public UNebulaFlowNavbarButton
{
	GENERATED_BODY()
	
protected:
	void OnButtonTriggered() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Param = "";

public:
	UFUNCTION(BlueprintCallable)
	void SetParam(FName NewParam) {Param = NewParam;};
};
