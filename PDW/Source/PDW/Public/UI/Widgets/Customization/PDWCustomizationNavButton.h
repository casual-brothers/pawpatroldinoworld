// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWUIBaseElement.h"
#include "PDWCustomizationNavButton.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWCustomizationNavButton : public UPDWUIBaseElement
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitButton"))
	void BP_InitButton(bool IsOnVehicle);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SwitchVechicleToPup"))
	void BP_SwitchVechicleToPup(bool IsOnVehicle);
};