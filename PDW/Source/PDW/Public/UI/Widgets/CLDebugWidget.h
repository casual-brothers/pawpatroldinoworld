// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CLDebugWidget.generated.h"

class UTextBlock;

UCLASS(MinimalAPI)
class UCLDebugWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void ShowCL();

	UFUNCTION()
	void HideCL();

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClLabel;

	void NativeConstruct() override;

};
