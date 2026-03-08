// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputModifiers.h"
#include "CustomInputModifierNegate.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "CustomNegate"))
class PDW_API UCustomInputModifierNegate : public UInputModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings)
	bool bX = true;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings)
	bool bY = true;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings)
	bool bZ = true;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
	virtual FLinearColor GetVisualizationColor_Implementation(FInputActionValue SampleValue, FInputActionValue FinalValue) const override;
};
